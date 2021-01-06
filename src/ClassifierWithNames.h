#ifndef CLASSIFIER_WITH_NAMES_H
#define CLASSIFIER_WITH_NAMES_H

#include <vector>
#include <map>
#include "Classifier.h"

namespace ice
{
  template<typename Tclassifier, typename Tlabel>
  class ClassifierWithNames
  {
  public:
    ClassifierWithNames() {};

  protected:
    Tclassifier cl;
    std::map<Tlabel, int> clIndex;
    std::vector<Tlabel> clName;

    int nLabels;
    int nFeatures;

    bool initialized;

    virtual int classNr(Tlabel classname) const
    {
      auto classIt = clIndex.find(classname);
      if (classIt != clIndex.end())
        {
          return classIt->second;
        }
      // std::cout << "undefined classname " << classname << std::endl;
      throw IceException("ClassifierWithNames::classNr","classname undefined");
    }

    virtual Tlabel className(int i) const
    {
      if (i >= 0 && i < (int)clName.size())
        {
          return clName[i];
        }
      throw IceException("ClassifierWithNames::classNr","class undefined");
    }

    virtual void init()
    {
      assert(nLabels == (int)clIndex.size());
      assert(nLabels == (int)clName.size());
      initialized = true;
      cl.Init(clIndex.size(), nFeatures);
    }

  public:
    ClassifierWithNames(int nFeatures): cl(), nLabels(0), nFeatures(nFeatures), initialized(false) {};

    ClassifierWithNames(const std::vector<Tlabel>& classnames, int nFeatures):
      cl(), nLabels(0), nFeatures(nFeatures), initialized(false)
    {
      for (Tlabel n : classnames)
        {
          addClass(n);
        }
    }

    virtual ~ClassifierWithNames() {};

#define FNAME "ClassName::addClass"
    virtual void addClass(Tlabel cln)
    {
      if (initialized)
        throw IceException(FNAME, M_ALREADY_INITIALIZED);
      clIndex[cln] = nLabels;
      clName.push_back(cln);
      nLabels++;
    }

    void addClass(Tlabel cln1, Tlabel cln2)
    {
      if (initialized)
        throw IceException(FNAME, M_ALREADY_INITIALIZED);

      for (Tlabel i = cln1; i < cln2; i++)
        {
          addClass(i);
        }
    }

    void addClass(const std::vector<Tlabel>& nl)
    {
      if (initialized)
        throw IceException(FNAME, M_ALREADY_INITIALIZED);

      for (Tlabel d : nl)
        {
          addClass(d);
        }
    }
#undef FNAME
    virtual int Train(Tlabel clname, const ice::Vector& feat)
    {
      if (!initialized)
        {
          init();
        }

      return cl.Train(classNr(clname), feat);
    }

    virtual int Train(const Matrix& features, const std::vector<Tlabel>& classn)
    {
      if (!initialized)
        {
          init();
        }

      IVector clnr;

      for (unsigned int i = 0; i < classn.size(); i++)
        {
          clnr.Append(classNr(classn[i]));
        }

      return cl.Train(features, clnr);
    }

    // verify classifier from classified sample list
    virtual double Test(const Matrix& m, const std::vector<Tlabel>& classname)
    {
      if (!initialized)
        {
          init();
        }

      IVector clnr;

      for (unsigned int i = 0; i < classname.size(); i++)
        {
          clnr.Append(classNr(classname[i]));
        }

      return cl.Test(m, clnr);
    }

    // classify single feature vector
    virtual Tlabel Classify(const ice::Vector& feat)
    {
      if (!initialized)
        {
          init();
        }

      return className(cl.Classify(feat));
    }

    virtual Tlabel Classify(const std::vector<double>& feat)
    {
      if (!initialized)
        {
          init();
        }

      return className(cl.Classify(feat));
    }

    // classify list of feature vectors and extend matrix with "class" column
    virtual int Classify(const Matrix& m, std::vector<Tlabel>& cln)
    {
      if (!initialized)
        {
          init();
        }

      IVector clnr;
      int rc = cl.Classify(m, clnr);
      cln.resize(clnr.size());

      for (unsigned int i = 0; i < clnr.size(); i++)
        {
          cln[i] = className(clnr[i]);
        }

      return rc;
    }

#if 0
    // store classifier to file
    virtual int Write(const string& fn);
    virtual int WriteC(const string& name);

    // restore classifier from file
    virtual int Read(const string& fn);
#endif

    virtual int Finish()
    {
      return cl.Finish();
    }
  };
}
#endif
