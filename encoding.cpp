//  2006-2008 (c) Viva64.com Team
//  2008-2020 (c) OOO "Program Verification Systems"

#include "encoding.h"
#include <cstring>
#include <cassert>
#include <ostream>
#ifdef _WIN32
#include "windows.h"
#endif

class Utf16Iterator
{
public:
  enum State
  {
      eStart,
      e2Bytes2,
      e3Bytes2,
      e3Bytes3
  };

  Utf16Iterator(const std::string &pBuf, size_t nSkip, Encoding eEncoding);
  unsigned char get() const { return m_cur; }
  void operator++();
  State getState() { return m_state; }
  operator bool() { return m_read <= m_end; }

protected:
  Encoding m_encoding;
  State m_state;
  unsigned char m_cur;
  unsigned short m_cur16;
  const unsigned char* m_buf;
  const unsigned char* m_read;
  const unsigned char* m_end;
};

Utf16Iterator::Utf16Iterator(const std::string& str, size_t nSkip, Encoding encoding)
{
  m_cur = 0;
  m_cur16 = 0;
  m_buf = reinterpret_cast<const unsigned char*>(&str[0]) + nSkip;
  m_read = m_buf;
  m_end = m_buf + str.length() - nSkip;
  m_encoding = encoding;
  m_state = eStart;
  operator++();
}

void Utf16Iterator::operator++()
{
  switch (m_state)
  {
    case eStart:
      if (m_encoding == Encoding::UTF16LE)
      {
        m_cur16 = *m_read++;
        m_cur16 |= static_cast<unsigned short>(*m_read << 8);
      }
      else
      {
        m_cur16 = static_cast<unsigned short>(*m_read++ << 8);
        m_cur16 |= *m_read;
      }
      ++m_read;

      if (m_cur16 < 0x80) {
        m_cur = static_cast<unsigned char>(m_cur16 & 0xFF);
        m_state = eStart;
      } else if (m_cur16 < 0x800) {
        m_cur = static_cast<unsigned char>(0xC0 | m_cur16 >> 6);
        m_state = e2Bytes2;
      } else {
        m_cur = static_cast<unsigned char>(0xE0 | m_cur16 >> 12);
        m_state = e3Bytes2;
      }
      break;
    case e2Bytes2:
    case e3Bytes3:
      m_cur = static_cast<unsigned char>(0x80 | (m_cur16 & 0x3F));
      m_state = eStart;
      break;
    case e3Bytes2:
      m_cur = static_cast<unsigned char>(0x80 | ((m_cur16 >> 6) & 0x3F));
      m_state = e3Bytes3;
      break;
  }
}

static Encoding DetermineEncoding(const char *buf, size_t bufLen, size_t &retSkip)
{
  struct BOM
  {
    Encoding enc;
    unsigned char buf[7];
    size_t len;
  };

  BOM boms[] = {
    {Encoding::UTF8,    {0xEF, 0xBB, 0xBF}, 3},
    {Encoding::UTF16BE, {0xFE, 0xFF},       2},
    {Encoding::UTF16LE, {0xFF, 0xFE},       2},
  };

  Encoding encoding = Encoding::UTF8;
  retSkip = 0;

  for (const BOM &bom : boms)
  {
    if (bufLen >= bom.len && memcmp(buf, reinterpret_cast<const char*>(bom.buf), bom.len) == 0)
    {
      retSkip = bom.len;
      return bom.enc;
    }
  }

#ifdef _WIN32
  int unicodeTest = IS_TEXT_UNICODE_STATISTICS;
  if (bufLen > 1 && buf[0] != 0 && buf[1] == 0 &&
      IsTextUnicode(buf, static_cast<int>(bufLen), &unicodeTest))
  {
    encoding = Encoding::UTF16LE;
    retSkip = 0;
  }
#endif

  return encoding;
}

void ConvertEncoding(std::string &str, Encoding &encoding, size_t &nSkip)
{
  nSkip = 0;

  encoding = DetermineEncoding(str.data(), str.size(), nSkip);
  switch (encoding)
  {
  case Encoding::ASCII:
  case Encoding::UTF8:
    if (nSkip != 0)
    {
      memmove(&str[0], &str[nSkip], str.length() - nSkip);
      str.resize(str.length() - nSkip);
    }
    break;
  case Encoding::UTF16BE:
  case Encoding::UTF16LE:
  {
    std::string newStr;
    newStr.reserve(str.size() + str.size() / 2 + 1);

    for (Utf16Iterator iter16(str, nSkip, encoding); iter16; ++iter16)
      newStr += static_cast<char>(iter16.get());

    str = std::move(newStr);
    break;
  }
  }
}

void ConvertEncoding(std::ostream& stream, const std::string &str, Encoding encoding, bool unix2dos)
{
  if (unix2dos)
  {
    std::string strLn;
    strLn.reserve(str.length() + 4);
    for (char ch : str)
    {
      if (ch == '\n')
        strLn += '\r';
      strLn += ch;
    }
    return ConvertEncoding(stream, strLn, encoding, false);
  }

  switch (encoding)
  {
  case Encoding::ASCII:
  case Encoding::UTF8:
    stream << str;
    break;
  case Encoding::UTF16BE:
    for (char ch : str)
    {
      assert(static_cast<unsigned char>(ch) < 0x80);
      stream << '\0' << ch;
    }
    break;
  case Encoding::UTF16LE:
    for (char ch : str)
    {
      assert(static_cast<unsigned char>(ch) < 0x80);
      stream << ch << '\0';
    }
    break;
  }
}
