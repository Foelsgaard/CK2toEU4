#include "Logger.hh"
#include <cassert>
#include "Parser.hh"

std::map<int, Logger*> Logger::logs;

LogStream const* const LogStream::Debug = new LogStream("debug");
LogStream const* const LogStream::Info = new LogStream("info");
LogStream const* const LogStream::Warn = new LogStream("warn");
LogStream const* const LogStream::Error = new LogStream("error");

LogOption const* const LogOption::Indent = new LogOption("indent", false);
LogOption const* const LogOption::Unindent = new LogOption("unindent", true);

Logger::Logger ()
  : active(true)
  , buffer()
  , precision(-1)
  , indent(0)
{}

Logger::~Logger () {}

Logger& Logger::append (unsigned int prec, double val) {
  static char str[1000];
  static char str2[1000];
  sprintf(str, "%%.%if", prec);
  sprintf(str2, str, val);
  buffer.append(str2);
  return *this;
}

Logger& Logger::operator<< (std::string dat) {
  if (!active) return *this;
  std::size_t linebreak = dat.find_first_of('\n');
  if (std::string::npos == linebreak) buffer.append(dat.c_str());
  else {
    buffer.append(dat.substr(0, linebreak).c_str());
    flush();
    (*this) << dat.substr(linebreak+1);
  }
  return *this;
}

Logger& Logger::operator<< (Object* dat) {
  if (!active) return *this;
  static int indent = 0;
  for (int i = 0; i < indent; i++) {
    *this << "  ";
  }
  if (dat->isLeaf()) {
    *this << dat->getKey() << " = " << dat->getLeaf() << "\n";
    return *this;
  }
  if (0 != dat->numTokens()) {
    *this << dat->getKey() << " = { " << dat->getLeaf() << " }\n";
    return *this;
  }

  if (dat != Parser::topLevel) {
    *this << dat->getKey() << " = { \n";
    indent++;
  }
  objvec leaves = dat->getLeaves();
  for (objiter i = leaves.begin(); i != leaves.end(); ++i) {
    *this << (*i);
  }
  if (dat != Parser::topLevel) {
    indent--;
    for (int i = 0; i < indent; i++) {
      *this << "  ";
    }
    *this << "} \n";
  }
  return *this;
}

Logger& Logger::operator<< (char* dat) {
  if (!active) return *this;
  return ((*this) << std::string(dat));
}

Logger& Logger::operator<< (const char* dat) {
  if (!active) return *this;
  return ((*this) << std::string(dat));
}

Logger& Logger::operator<< (QString dat) {
  if (!active) return *this;
  int linebreak = dat.indexOf('\n');
  if (-1 == linebreak) buffer.append(dat);
  else {
    buffer.append(dat.mid(0, linebreak));
    flush();
    (*this) << dat.mid(linebreak+1);
  }

  return *this;
}

Logger& Logger::operator<< (int dat) {
  if (!active) return *this;
  buffer.append(QString("%1").arg(dat));
  return *this;
}

Logger& Logger::operator<< (unsigned int dat) {
  if (!active) return *this;
  buffer.append(QString("%1").arg(dat));
  return *this;
}

Logger& Logger::operator<< (double dat) {
  if (!active) return *this;
  if (precision > 0) append(precision, dat);
  else buffer.append(QString("%1").arg(dat));
  return *this;
}

Logger& Logger::operator<< (char dat) {
  if (!active) return *this;
  if ('\n' == dat) flush();
  else buffer.append(dat);
  return *this;
}

Logger& Logger::operator<< (LogOption const* const opt) {
  if (opt == LogOption::Indent) indent += 2;
  else if (opt == LogOption::Unindent) {
    indent -= 2;
    if (0 > indent) indent = 0;
  }
  return *this;
}

Logger* Logger::createStream (int idx) {
  logs[idx] = new Logger();
  return logs[idx];
}

Logger* Logger::createStream (LogStream const* const str) {
  logs[*str] = new Logger();
  return logs[*str];
}

Logger& Logger::logStream (int idx) {
  assert(logs[idx]);
  return *(logs[idx]);
}

Logger& Logger::logStream (LogStream const* const str) {
  assert(logs[*str]);
  return *(logs[*str]);
}

Logger& Logger::logStream (LogStream const& str) {
  assert(logs[str]);
  return *(logs[str]);
}

Logger& Logger::logStream (const string& ls) {
  LogStream const* const str = LogStream::findByName(ls);
  return *(logs[*str]);
}

std::ostream& Logger::flush () {
  if (0 < indent) emit message(QString(indent, ' ') + buffer);
  else emit message(buffer);
  buffer.clear();
  return *this;
}
