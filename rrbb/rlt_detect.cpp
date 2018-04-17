#include "rlt_detect.hpp"
#include <list>
#include <opencv2/opencv.hpp>
#include <boost/geometry.hpp>
#include "utility.hpp"
#include "image_util.hpp"
#include "image_primitives.hpp"

using namespace std;
using namespace cv;
namespace bgi = boost::geometry::index;

list<Rect> findRLT(Mat& text, Mat& nontext){
  int vl = nontext.rows/31, hl = nontext.cols/31;
  Mat elementV = getStructuringElement(MORPH_RECT, Size(1, vl), Point(-1, -1));
  Mat elementH = getStructuringElement(MORPH_RECT, Size(hl, 1), Point(-1, -1));
  Mat vertical, horizontal;
  lineSep(nontext, vertical, elementV);
  lineSep(nontext, horizontal, elementH);
  Mat intersect = vertical & horizontal;
  Mat lines = vertical | horizontal;
  list<Rect> ib;
  boundingVector(intersect, back_inserter(ib));
  Mat cc;
  vector<ComponentStats> stats = statistics(nontext, cc);
  sort(stats.begin(), stats.end(), [](const ComponentStats& lh, const ComponentStats& rh){return lh.bb_area > rh.bb_area;});
  bgi::rtree<Rect, bgi::quadratic<16> > tree(ib);
  list<Rect> tables;
  bgi::rtree<Rect, bgi::quadratic<16> > tableTree;
  for (ComponentStats& cs : stats){
    Rect& r = cs.r;
    Mat textRegion = text(r);
    Mat nontextRegion = nontext(r);
    Mat invertedLines;
    bitwise_not(lines(r), invertedLines);
    if (distance(tableTree.qbegin(bgi::intersects(r)), tableTree.qend()) == 0 &&
	distance(tree.qbegin(bgi::intersects(r)), tree.qend()) >= 10 &&
	verticalArrangement(invertedLines)){
      Mat tmp;
      bitwise_xor(nontextRegion, lines(r), tmp);
      list<TextLine> tls = findLines(text);
      if (!hasLargeGraphElement(tmp) ||
	  verticalArrangement(text, tls)){
	tables.push_back(r);
	tableTree.insert(r);
      }
    }
  }
  return tables;
}
