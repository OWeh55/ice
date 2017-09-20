#include <fstream>
#include <iostream>

#include "string.h"
#include "parser.h"
#include "file.h"

#include "f_constant.h"
#include "f_macro.h"
#include "f_dial.h"

#include "makefilter.h"

#include "compile.h"

Library macros;

void compileParameter(Parser &line,
                      const OutletMap &outlets,
                      const FilterOutlet &lastOutlet,
                      std::vector<FilterOutlet> &input,
                      bool scriptVerbose)
{
  int inputIndex = 0;

  if (line.nextToken().token == ":")
    line.skipToken();

  if (line.empty() || line.nextToken().token == "=")
    {
      // if parameterstring is empty use outlet 0
      // of previous defined filter
      if (lastOutlet.inp == NULL)
        throw SdpException("Parsing", "no default stream from previous filter");
      input[0] = lastOutlet;
    }
  else
    {
      if (scriptVerbose)
        cout << ":";

      while (!line.empty() && line.nextToken().token != "=")
        {
          if (inputIndex >= (int)input.size())
            throw SdpException("Parameter", "too many parameters");

          if (scriptVerbose)
            {
              if (inputIndex > 0)
                cout << ",";
            }

          Token para = line.getToken();
          // cout << "token: " << para.token << " " << para.type << endl;
          if (para.token != ",") // parameter not empty
            {
              if (para.token == "(") // parameter is vector
                {
                  if (scriptVerbose)
                    cout << "(";

                  Token firstvalue = line.getToken(Token::integer | Token::floatingpoint | Token::stringliteral);
                  if (scriptVerbose)
                    cout << firstvalue.token;

                  switch (firstvalue.type)
                    {
                    case Token::integer:
                    {
                      std::vector<int> ivv;
                      ivv.push_back(firstvalue.getInt());
                      while (line.nextToken().token != ")")
                        {
                          line.expect(',');
                          Token nextvalue = line.getToken(Token::integer);
                          ivv.push_back(nextvalue.getInt());
                          if (scriptVerbose)
                            cout << "," << nextvalue.token;
                        }
                      Filter *in = new ConstantFilter(ivv);
                      input[inputIndex] = FilterOutlet(in);
                    }
                    break;

                    case Token::floatingpoint:
                    {
                      std::vector<double> dvv;
                      dvv.push_back(firstvalue.getFloat());
                      while (line.nextToken().token != ")")
                        {
                          line.expect(',');
                          Token nextvalue = line.getToken(Token::integer | Token::floatingpoint);
                          dvv.push_back(nextvalue.getFloat());
                          if (scriptVerbose)
                            cout << "," << nextvalue.token;
                        }
                      Filter *in = new ConstantFilter(dvv);
                      input[inputIndex] = FilterOutlet(in);
                    }
                    break;

                    case Token::stringliteral:
                    {
                      std::vector<string> svv;
                      svv.push_back(firstvalue.getString());
                      while (line.nextToken().token != ")")
                        {
                          line.expect(',');
                          Token nextvalue = line.getToken(Token::stringliteral);
                          svv.push_back(nextvalue.getString());
                          if (scriptVerbose)
                            cout << "," << nextvalue.token;
                        }
                      Filter *in = new ConstantFilter(svv);
                      input[inputIndex] = FilterOutlet(in);
                    }
                    break;
                    }
                  line.expect(')');
                  if (scriptVerbose)
                    cout << ")";
                }
              else
                {
                  if (scriptVerbose)
                    {
                      cout << para.token;
                    }

                  switch (para.type)
                    {
                    case Token::integer:
                    {
                      Filter *in = new ConstantFilter(para.getInt());
                      input[inputIndex] = FilterOutlet(in);
                    }
                    break;

                    case Token::floatingpoint:
                    {
                      Filter *in = new ConstantFilter(para.getFloat());
                      input[inputIndex] = FilterOutlet(in);
                    }
                    break;
                    case Token::stringliteral:
                    {
                      Filter *in = new ConstantFilter(para.getString());
                      input[inputIndex] = FilterOutlet(in);
                    }
                    break;
                    case Token::identifier:
                    {
                      string label = para.token;
                      if (outlets.count(label) == 0)
                        throw SdpException("Parsing", "Unknown label \"" + label + "\"");
                      input[inputIndex] = (outlets.find(label))->second;
                    }
                    break;
                    default:

                      throw SdpException("Parsing", "wrong parameter format " + para.token);
                    }
                }
              Token tn = line.nextToken();

              if (tn.token == ",")
                line.skipToken();
            }
          else
            {
              // if the _first_ input is empty, then use
              // "lastOutlet" here
              // for other inputs NULL means undefined or default
              if (inputIndex == 0)
                input[0] = lastOutlet;
            }
          ++inputIndex;
        }
    }
}

/**
 * compile whole program or macros
 *
 * compilation consumes FilterDescription
 * returns filter tree
 */

Filter *Compile(FilterDescription &src,
                OutletMap &outlet,  // predefined outlets for input of macro,
                // used to add other labeled outlets during compilation
                bool scriptVerbose
               )
{
  bool macroMode = false;
  string macroName;
  Macro newMacro;

  FilterOutlet lastOutlet;
  Filter *result = NULL;
  while (!src.empty())
    {
      // prepare first operation for parsing
      string currentLine = src.front();
      // .. and drop it
      src.pop_front();

      Parser oper(currentLine);

      Token filterToken;
      // first part is filter name
      try
        {
          filterToken = oper.getToken(Token::identifier);
        }
      catch (SdpException exc)
        {
          throw SdpException("Parsing", "Filter name expected: " + currentLine);
        }

      string filter = filterToken.token;

      // special pseudo filter names
      if (stringMatch(filter, "include"))
        {
          // include file
          oper.expect(":");

          vector<string> ops;

          string fn = oper.getToken(Token::stringliteral).getString();

          fn = findFile(fn, pathlist);
          //  cout << "Öffne Datei \"" << fn << "\"" << endl;
          string line;
          ifstream is(fn);
          if (!is.good())
            throw FileException("include", fn);
          while (getline(is, line))
            {
              line = trim(line);
              if (!line.empty() && line[0] != '#')
                ops.push_back(line);
            }
          while (!ops.empty())
            {
              src.push_front(ops.back());
              ops.pop_back();
            }
          // for (int i=0;i<operations.size();++i)
          // cout << ">>>" << operations[i]<< "<<<" <<endl;
        }
      else if (stringMatch(filter, "define"))
        {
          if (scriptVerbose)
            cout << "define:";

          oper.expect(":");
          string name = oper.getToken(Token::identifier).token;

          if (scriptVerbose)
            cout << name;

          while (!oper.empty() && oper.nextToken().token != "=")
            {
              oper.expect(",");
              string fpara = oper.getToken(Token::identifier).token;
              if (scriptVerbose)
                cout << "," << fpara;
              newMacro.inputlabels.push_back(fpara);
            }
          macroName = name;
          macroMode = true;
          if (scriptVerbose)
            cout << endl;
        }
      else if (stringMatch(filter, "return"))
        {
          if (!macroMode)
            throw SdpException("Parsing", "return outside macro definition");

          if (scriptVerbose)
            cout << "  Return";

          if (!oper.empty() && oper.nextToken().token != "=")
            throw SdpException("Parsing", "no parameter for return allowed! forgot '=' ?");

          if (oper.nextToken().token == "=")
            {
              oper.skipToken();

              string fpara = oper.getToken(Token::identifier).token;
              if (scriptVerbose)
                cout << "=" << fpara;

              newMacro.outputlabels.push_back(fpara);

              while (!oper.empty())
                {
                  oper.expect(",");
                  fpara = oper.getToken(Token::identifier).token;
                  if (scriptVerbose)
                    cout << "," << fpara;
                  newMacro.outputlabels.push_back(fpara);
                }
            }

          macros[macroName] = newMacro;
          newMacro.clear();
          macroMode = false;
          if (scriptVerbose)
            cout << endl;
        }
      else
        {
          if (macroMode)
            {
              newMacro.fd.push_back(currentLine);
              if (scriptVerbose)
                cout << "  " << currentLine << endl;
            }
          else
            {
              Filter *currentFilter = NULL;
              if (stringMatch(filter, "dial"))
                {
                  // very special filter which needs special handling
                  // interactive filter Dial(int val,int min,int max,int step)
                  oper.expect(':');
                  string name = oper.getToken(Token::stringliteral).getString();
                  oper.expect(',');
                  int initialValue = oper.getToken(Token::integer).getInt();
                  oper.expect(',');
                  int min = oper.getToken(Token::integer).getInt();
                  oper.expect(',');
                  int max = oper.getToken(Token::integer).getInt();
                  oper.expect(',');
                  int stepWidth = oper.getToken(Token::integer).getInt();
                  Dial *dial = new Dial(initialValue, min, max, stepWidth);
                  dial->setName(name);
                  currentFilter = dial;
                  dials.push_back(dial);
                }
              else if (macros.count(filter) > 0) // macro exists
                {
                  // this is a macro call

                  if (scriptVerbose)
                    {
                      cout << filter ;
                    }

                  const Macro &currentMacro = macros[filter];

                  OutletMap outletMap;
                  int nInputs = currentMacro.inputlabels.size();
                  int nOutputs = currentMacro.outputlabels.size();
                  if (nOutputs == 0)
                    nOutputs = 1;

                  MacroCapsule *macro = new MacroCapsule(nInputs, nOutputs);
                  macro->setName(filter);

                  std::vector<FilterOutlet> macroinputs(nInputs);
                  try
                    {
                      compileParameter(oper, outlet, lastOutlet,
                                       macroinputs,
                                       scriptVerbose);
                      for (int i = 0; i < nInputs; ++i)
                        macro->setInput(macroinputs[i], i + nOutputs);
                    }
                  catch (SdpException exc)
                    {
                      exc.setWhere("Macro " + filter);
                      throw exc;
                    }

                  // assign input names
                  for (int i = 0; i < (int)nInputs; ++i)
                    {
                      outletMap[currentMacro.inputlabels[i]] = FilterOutlet(macro, i + nOutputs);
                    }

                  FilterDescription fd = macros[filter].fd;
                  currentFilter = Compile(fd, outletMap, false);
                  //  for (auto it=outletMap.begin();it != outletMap.end();it++)
                  //    cout << it->first << endl;
                  if (!currentMacro.outputlabels.empty())
                    {
                      for (int i = 0; i < nOutputs; ++i)
                        {
                          string outlet = currentMacro.outputlabels[i];
                          if (outletMap.count(outlet) == 0)
                            throw SdpException("Macro " + filter, "Outlet " + outlet + " undefined");
                          macro->setInput(outletMap[outlet], i);
                        }
                    }
                  else
                    {
                      // default output = output 0 of last filter
                      macro->setInput(currentFilter, 0);
                    }
                  currentFilter = macro;
                }
              else
                {
                  // find filter by name and create it
                  currentFilter = mkFilter(filter);

                  if (scriptVerbose)
                    cout << currentFilter->getName();

                  try
                    {
                      compileParameter(oper, outlet, lastOutlet, currentFilter->getInput(), scriptVerbose);
                    }
                  catch (SdpException exc)
                    {
                      exc.setWhere("Filter " + currentFilter->getName());
                      throw exc;
                    }

                  /*
                  // add display for debugging
                  if (debug)
                  {
                  Filter *deb = new ImageDisplay();
                  deb->setInput(currentFilter,0,0);
                  // filter + " : " + parastring);
                  }
                  */
                }

              // register results
              if (oper.nextToken().token == "=")
                {
                  oper.skipToken();
                  if (scriptVerbose)
                    cout << "=" ;

                  // here names are assigned to the outlets of the filter
                  int labelindex = 0;
                  string fname = oper.getToken(Token::identifier).token;
                  // we allow to overwrite older names here
                  // this allows easy insertion of filters (filter:image=image)
                  outlet[fname] = FilterOutlet(currentFilter, labelindex);

                  if (scriptVerbose)
                    {
                      cout << fname;
                    }

                  labelindex++;

                  while (!oper.empty())
                    {
                      oper.expect(",");
                      fname = oper.getToken(Token::identifier).token;
                      outlet[fname] = FilterOutlet(currentFilter, labelindex);
                      if (scriptVerbose)
                        {
                          cout << "," << fname;
                        }
                      labelindex++;
                    }
                }
              lastOutlet = FilterOutlet(currentFilter, 0);

              result = currentFilter;
              if (scriptVerbose)
                cout << endl;
            }
        }
    }
  if (macroMode)
    throw SdpException("Compile", "unfinished macro");
  return result;
}

void compileMacro(const std::string &fn, bool scriptverbose)
{
  deque<string> ops;
  string line;
  ifstream is(fn);
  if (!is.good())
    throw FileException("compileMacro", fn);

  while (getline(is, line))
    {
      line = trim(line);
      if (!line.empty() && line[0] != '#')
        ops.push_back(line);
    }

  OutletMap om;
  Filter *res = Compile(ops,
                        om,  // outletmap empty for macrodefinitions
                        scriptverbose
                       );

  if (res != NULL)
    throw SdpException("compileMacro", "file " + fn + " contains non-macro-definitions");
}
