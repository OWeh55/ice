#include <image.h>

#define xyind(x,y) (y*xs+x)
#define indx(idx) (idx % xs)
#define indy(idx) (idx / xs)

#if 0
class DisjointSets
// Disjoint sets, bei denen jedes Element in eine Menge gehört, im Startzustand
// bildet jedes Element eine eigene Menge
{
private:
  mutable unsigned int* sarray;
  unsigned int size;

  // block copy constructor
private:
  DisjointSets(const DisjointSets& s) ;

public:
  void Init()
  {
    // initialisation - "no set yet"
    for (unsigned int i = 0; i < size; i++)
      sarray[i] = i;
  }

  DisjointSets(unsigned int s)
  {
    sarray = new unsigned int[s];
    size = s;
    Init();
  }

  ~DisjointSets()
  {
    delete [] sarray;
  }

  unsigned int Find(unsigned int x)
  {
    unsigned int r = sarray[x];

    if (r != x)
      {
        r = Find(r);
        sarray[x] = r;
      }

    return r;
  }

  unsigned int LinkR(unsigned int b1, unsigned int b2)
  {
    sarray[b2] = b1;
    return b1;
  }

  unsigned int Link(unsigned int s1, unsigned int s2)
  {
    return LinkR(Find(s1), Find(s2));
  }

}; // class DisjointSets
#endif
class node
{
  typedef vector<node*> nodelist;

  // eher sowas wie struct, sollte später verbessert werden
private:
  node(const node&);
public:
  int level;

  nodelist children;
  int area;
  int highest;

  node(): level(0), area(0), highest(0) {}
  node(int levelp): level(levelp), area(1), highest(level) {}

private:
  void addChilds(const node& ch)
  {
    nodelist::const_iterator it;

    for (it = ch.children.begin(); it != ch.children.end(); it++)
      children.push_back(*it);
  }

public:
  void Merge(const node& ad)
  {
    addChilds(ad);
    area += ad.area;

    if (ad.highest > highest)
      highest = ad.highest;
  }

  //  void addChild(node *ch) { children.push_back(ch); }
  void addAsChild(node& ch)
  {
    children.push_back(&ch);
    area += ch.area;

    if (ch.highest > highest)
      highest = ch.highest;
  }

  void print(int depth, int lvl = 0)
  {
    if (depth > lvl)
      {
        for (int i = 0; i < lvl; i++) cout << "*";

        cout << area << " " << level << " " << highest << "  " << children.size() << endl;

        nodelist::const_iterator it;

        for (it = children.begin(); it != children.end(); it++)
          (*it)->print(depth, lvl + 1);
      }
  }
};

void usage(const string& pn)
{
  cout << "Usage: " << pn << " <options> [<filename>] " << endl;
  cout << "options:" << endl;
  cout << " -d <n> Darstellungen (Bit-Werte)" << endl;
  cout << "   -d 1 Originalbild" << endl;
  cout << "   -d 2 Regionen gleichen Wertes" << endl;
  cout << "   -d 4 Komponenten-Baum" << endl;
  cout << " -m <n> Maskierung aller Grauwerte - AND n" << endl;
  cout << " -h  help" << endl;
  exit(1);
}

inline bool InRange(int max1, int val)
{
  return val >= 0 && val < max1;
}

int main(int argc, char** argv)
{
  string fn = "test_gray.jpg";
  int display = 2;
  int mask = -1;
  int print = 0;
  bool smear = false;
  int rc;

  while ((rc = getopt(argc, argv, "hd:m:sp:")) >= 0)
    {
      switch (rc)
        {
        case 'd':
          display = atol(optarg);
          break;
        case 'm':
          mask = atol(optarg);
          break;
        case 'p':
          print = atol(optarg);
          break;
        case 's':
          smear = true;
          break;
        case 'h': // no break !!
        default:
          usage(argv[0]);
          break;
        }
    }

  if (optind < argc)
    fn = argv[optind];

  int xs, ys, mv, ch;

  InfImgFile(fn, xs, ys, mv, ch);

  Image img1 = NewImg(xs, ys, mv);
  ReadImg(fn, img1);
  Image mrk = NewImg(img1);
  ClearImg(mrk);

  int ss = xs * ys;

  if (display & 1)
    Show(ON, img1, "Ausgangsbild");

  if (smear)
    SmearImg(img1, img1, 5, 5);

  if (mask > 0)
    {
      for (int y = 0; y < ys; y++)
        for (int x = 0; x < xs; x++)
          PutVal(img1, x, y, GetVal(img1, x, y) & mask);
    }

  // component tree entsprechend najman und couprie

  DisjointSets Node(ss);
  DisjointSets Tree(ss);

  int rootnode;

  vector<int> lowestNode(ss);
  vector<node*> nodes(ss);

  // Vorbereitung der Datenstrukturen und Sortieren der Grauwerte

  vector<int> slink(ss);
  vector<int> bucket(mv + 1);

  for (int i = 0; i <= mv; i++)
    bucket[i] = -1;

  for (int y = 0; y < ys; y++)
    for (int x = 0; x < xs; x++)
      {
        int idx = xyind(x, y);
        int g = GetVal(img1, x, y);

        nodes[idx] = NULL;
        lowestNode[idx] = idx;

        slink[idx] = bucket[g];
        bucket[g] = idx;
      }

#if 0

  for (int g = mv; g >= 0; g--)
    {
      Printf("Grauwert: %d\n", g);
      int idx = bucket[g];

      while (idx > = 0)
        {
          PutVal(mrk, indx(idx), indy(idx), 0);
          idx = slink[idx];
        }

      Delay(0.1);
    }

#endif

  Visual prog;

  if (display & 4)
    prog = Show(OVERLAY, img1, mrk, "Teilbäume");

  for (int level = mv; level >= 0; level--)
    {
      vector<int> ptrees;
      Printf("Grauwert: %d\n", level);
      int idx = bucket[level];

      while (idx >= 0)
        {
          // hier Punkt behandeln
          int px = indx(idx);
          int py = indy(idx);
          int curTree = Tree.Find(idx);
          int curNode = Node.Find(lowestNode[curTree]);

          nodes[idx] = new node(level);

          if (display & 4)
            PutVal(mrk, px, py, 1);

          // foreach already processed neighbor
          for (int dir = 0; dir < 8; dir += 2)
            {
              int qx, qy;
              Freeman(dir).move(px, py, qx, qy);

              if (InRange(xs, qx) && InRange(ys, qy))
                {
                  int qidx = xyind(qx, qy);

                  if (nodes[qidx] != NULL) // punkt bearbeitet
                    {
                      if (display & 4)
                        PutVal(mrk, qx, qy, 2);

                      int adjTree = Tree.Find(qidx);
                      int adjNode = Node.Find(lowestNode[adjTree]);

                      if (curNode != adjNode)
                        {
                          if (nodes[curNode]->level == nodes[adjNode]->level)
                            {
                              // curNode = MergeNodes(adjNode,curNode);
                              Node.Link(curNode, adjNode);
                              nodes[curNode]->Merge(*nodes[adjNode]);
                              delete nodes[adjNode];
                            }
                          else
                            {
                              nodes[curNode]->addAsChild(*nodes[adjNode]);
                            }

                          curTree = Tree.Link(adjTree, curTree);
                          lowestNode[curTree] = curNode;
                        }
                    }
                }
            }

          // nächster Punkt
          idx = slink[idx];
        }
    }

  if (display & 4)
    {
      GetChar();
      Show(OFF, prog);
    }

  rootnode = lowestNode[Tree.Find(Node.Find(0))];

  cout << rootnode << endl;
//  cout << nodes[rootnode]->area << " == " << ss << endl;
//  cout << nodes[rootnode]->level << " == " << nodes[rootnode]->highest << endl;

  if (print > 0)
    nodes[rootnode]->print(print, 0);

  if (display & 2)
    {
      Show(OVERLAY, img1, mrk, "");
      ClearImg(mrk);
      vector<int> M(ss);

      for (int idx = 0; idx < ss; idx++)
        {
          M[idx] = Node.Find(idx);

          if (nodes[M[idx]]->area > 100)
            PutVal(mrk, indx(idx), indy(idx), M[idx] % 255 + 1);
        }

      GetChar();
    }

  return 1;
}
