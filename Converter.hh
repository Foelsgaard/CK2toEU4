#ifndef CONVERTER_HH
#define CONVERTER_HH

#include <QThread>
#include <fstream>
#include <map>
#include <string>
#include <queue>

#include "UtilityFunctions.hh"

using namespace std;

class ConverterJob : public Enumerable<const ConverterJob> {
public:
  ConverterJob (string n, bool final) : Enumerable<const ConverterJob>(this, n, final) {}
  static ConverterJob const* const Convert;
  static ConverterJob const* const LoadFile;
};

class Object;
class Window;
class Converter : public QThread {
public:
  Converter (Window* ow, string fname);
  ~Converter ();
  void scheduleJob (ConverterJob const* const cj) {jobsToDo.push(cj);}

protected:
  void run ();

private:
  // Misc globals
  string targetVersion;
  string sourceVersion;
  string ck2FileName;
  Object* ck2Game;
  Object* eu4Game;
  queue<ConverterJob const*> jobsToDo;
  Object* configObject;

  // Conversion processes

  // Infrastructure
  void loadFile ();
  void convert ();
  void configure ();

  // Initialisers
  bool createCountryMap ();
  bool createProvinceMap ();
  void loadFiles ();

  // Helpers:
  void cleanUp ();
  Object* loadTextFile (string fname);
  bool swapKeys (Object* one, Object* two, string key);

  // Maps

  // Lists

  // Input info
  Object* provinceMapObject;
  Object* countryMapObject;
  Object* customObject;

  Window* outputWindow;
};

#endif

