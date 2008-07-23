/* hex.cc - Reading and writing intel hex and S19 files.
 * Copyright 2007-2008 Bas Wijnen <wijnen@debian.org>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "asm.hh"
#include <shevek/error.hh>

namespace
{
	int rd_digit (char data)
	{
		if (data >= '0' && data <= '9')
			return data - '0';
		if (data >= 'A' && data <= 'F')
			return data - 'A' + 10;
		if (data >= 'a' && data <= 'f')
			return data - 'a' + 10;
		shevek_error ("invalid character in number: " << data);
		return -1;
	}

	int rd_byte (char const *data)
	{
		int h = rd_digit (data[0]);
		int l = rd_digit (data[1]);
		if (h == -1 || l == -1)
			return -1;
		return (h << 4) | l;
	}

	struct line_t
	{
		unsigned addr;
		std::vector <int> data;
		line_t () : addr (0) {}
	};

	line_t rd_intel_line (std::string const &line, unsigned &linear,
			unsigned &segment, bool &eof)
	{
		line_t result;
		if (line.size () < 11)
		{
			shevek_error ("line too short");
			return result;
		}
		if (line[0] != ':')
		{
			shevek_error ("line doesn't start with ':'");
			return result;
		}
		unsigned len = rd_byte (&line.data ()[1]);
		if (line.size () != (len << 1) + 11)
		{
			shevek_error ("line has incorrect size ("
					<< line.size () << " != "
					<< ((len << 1) + 11));
			return result;
		}
		result.data.resize (len);
		result.addr = (rd_byte (&line.data ()[3]) << 8)
			+ rd_byte (&line.data ()[5]);
		unsigned check = 0;
		for (unsigned i = 0; i < len + 4; ++i)
		{
			check += rd_byte (&line.data ()[(i << 1) + 1]);
		}
		unsigned checksum = rd_byte (&line.data ()[( (len + 4) << 1) + 1]);
		if ( ( (checksum + check) & 0xff) != 0)
		{
			shevek_error ("invalid checksum (" << checksum << " + " << check << " & 0xff != 0)");
			return result;
		}
		unsigned type = rd_byte (&line.data ()[7]);
		for (unsigned i = 0; i < len; ++i)
		{
			result.data[i] = rd_byte (&line.data ()[(i << 1) + 9]);
		}
		switch (type)
		{
		case 0: // data
			result.addr += segment + linear;
			return result;
		case 1: // end of file
			eof = true;
			if (result.data.size () != 0)
			{
				shevek_error ("invalid EOF record");
				result.data.clear ();
			}
			return result;
		case 2: // extended segment address
			if (result.data.size () != 2)
			{
				shevek_error
					("invalid extended segment record");
				result.data.clear ();
				return result;
			}
			segment = (result.data[0] << 4)
				| (result.data[1] << 12);
			result.data.clear ();
			return result;
		case 4: // extended linear address
			if (result.data.size () != 2)
			{
				shevek_error
					("invalid extended linear record");
				result.data.clear ();
				return result;
			}
			linear = (result.data[0] << 16)
				| (result.data[1] << 24);
			result.data.clear ();
			return result;
		case 3: // start segment address
		case 5: // start linear address
			shevek_warning ("ignoring start address record");
			result.data.clear ();
			return result;
		default:
			shevek_error ("invalid record type " << type);
			result.data.clear ();
			return result;
		}
	}

	line_t rd_s19_line (std::string const &line, bool &eof)
	{
		line_t result;
		if (line.size () < 6)
		{
			shevek_error ("line too short for s19 file");
			return result;
		}
		if (line[1] != '1' && line[1] != '9')
		{
			shevek_warning ("unparsed line: " << line);
			return result;
		}
		unsigned len = rd_byte (&line.data ()[2]);
		if (len * 2 + 4 != line.size ())
		{
			shevek_error ("line has incorrect length in s19 file: " << line);
			return result;
		}
		int h = rd_byte (&line.data ()[4]);
		int l = rd_byte (&line.data ()[6]);
		if (h < 0 || l < 0)
		{
			shevek_error ("invalid address in " << line);
			return result;
		}

		// Correct for address bytes and checksum
		len -= 3;

		result.addr = (h << 8) + l;
		result.data.resize (len);
		unsigned checksum (len + l + h);
		for (unsigned i = 0; i < len; ++i)
		{
			unsigned d = rd_byte (&line.data ()[8 + 2 * i]);
			checksum += d;
			result.data[i] = d;
		}
		if ((checksum + rd_byte (&line.data ()[8 + 2 * len])) & 0xff != 0)
		{
			shevek_error ("incorrect checksum on line: " << line);
			return result;
		}
		if (line[1] == '9')
		{
			eof = true;
			if (result.data.size () != 0)
				shevek_error ("invalid EOF record: " << line);
			return result;
		}
		return result;
	}

	line_t rd_line (std::string const &line, unsigned &linear,
			unsigned &segment, bool &eof)
	{
		switch (line[0])
		{
			case ':':
				return rd_intel_line (line, linear, segment, eof);
			case 'S':
				return rd_s19_line (line, eof);
			default:
				shevek_error ("invalid hex file (not intel and not s19");
				return line_t ();
		}
	}

	std::list <line_t> read_lines (std::istream &s)
	{
		unsigned linear (0), segment (0);
		bool eof (false);
		std::list <line_t> result;
		std::string line;
		while (std::getline (s, line) )
		{
			if (!line.empty () && line[line.size () - 1] == '\r')
				line = line.substr (0, line.size () - 1);
			if (eof)
			{
				shevek_error ("data after eof marker");
				return result;
			}
			line_t l = rd_line (line, linear, segment, eof);
			if (l.data.size () == 0)
				continue;
			std::list <line_t>::iterator i;
			for (i = result.begin (); i != result.end (); ++i)
			{
				if (l.addr >= i->addr + i->data.size () )
					continue;
				if (l.addr + l.data.size () <= i->addr)
					break;
				shevek_error ("duplicate data defined");
				result.clear ();
				return result;
			}
			result.insert (i, l);
		}
		return result;
	}

	std::string make_hex (int num, int size = 1)
	{
		std::string ret;
		ret.resize (2 * size);
		char const *digit = "0123456789ABCDEF";
		for (unsigned i = 0; i < size; ++i)
		{
			int n = num >> ((size - i - 1) * 8);
			ret[2 * i] = digit[(n >> 4) & 0xf];
			ret[2 * i + 1] = digit[n & 0xf];
		}
		return ret;
	}

	void write_record (std::ostream &file, int type,
			std::string const &data = std::string (), int addr = 0)
	{
		int sum = type + addr + (addr >> 8);
		file << ':' << make_hex (data.size ()) << make_hex (addr, 2)
			<< make_hex (type);
		for (unsigned i = 0; i < data.size (); ++i)
		{
			sum += data[i];
			file << make_hex (data[i]);
		}
		file << make_hex (-sum) << '\n';
	}
}

void Hex::open (std::istream &file)
{
	std::list <line_t> lines = read_lines (file);
	while (!lines.empty ())
	{
		if (lines.front ().addr + lines.front ().data.size ()
				> data.size ())
			data.resize (lines.front ().addr
					+ lines.front ().data.size (), -1);
		std::copy (lines.front ().data.begin (),
				lines.front ().data.end (),
				data.begin () + lines.front ().addr);
		lines.pop_front ();
	}
}

void Hex::write (std::ostream &file)
{
	int high = 0;
	std::string tosend;
	unsigned base_addr = 0;
	for (unsigned a = 0; a < data.size (); ++a)
	{
		if (data[a] == -1)
		{
			if (!tosend.empty ())
			{
				write_record (file, 0, tosend, base_addr);
				tosend.clear ();
			}
			continue;
		}
		if (a >= (high + 1) << 16)
		{
			if (!tosend.empty ())
			{
				write_record (file, 0, tosend, base_addr);
				tosend.clear ();
			}
			high = a >> 16;
			std::string str;
			str.resize (2);
			str[0] = high & 0xff;
			str[1] = (high >> 8) & 0xff;
			write_record (file, 2, str);
		}
		if (tosend.empty ())
			base_addr = a;
		tosend += data[a];
		if (tosend.size () == 0x20)
		{
			write_record (file, 0, tosend, base_addr);
			tosend.clear ();
		}
	}
	if (!tosend.empty ())
		write_record (file, 0, tosend, base_addr);
	write_record (file, 1);
}

int Hex::get (unsigned address) const
{
	if (data.size () <= address)
		return -1;
	return data[address];
}

void Hex::set (unsigned address, int value)
{
	if (data.size () <= address)
		data.resize (address + 1, -1);
	data[address] = value;
}
