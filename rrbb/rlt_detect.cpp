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
  int vl = nontext.rows/57, hl = nontext.cols/57;
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
  list<Rect> textBound;
  bgi::rtree<Rect, bgi::quadratic<16> > tree(ib), tableTree;
  list<Rect> tables;
  for (ComponentStats& cs : stats){
    Rect& r = cs.r;
    Mat textRegion = text(r), nontextRegion = nontext(r), invertedLines;
    bitwise_not(lines(r), invertedLines);
    erode(invertedLines, invertedLines, getStructuringElement(MORPH_RECT, Size(1, 5), Point(-1, -1)));
    if (tableTree.qbegin(bgi::intersects(r)) == tableTree.qend() &&
	distance(tree.qbegin(bgi::intersects(r)), tree.qend()) >= 10){
      Mat tmp;
      bitwise_xor(nontextRegion, lines(r), tmp);
      list<TextLine> tls = findLines(text);      
      if (!hasLargeGraphElement(tmp) &&
	  isTableLike(invertedLines) &&
	  lowCrapRatio(textRegion, tmp) &&
	  !hasOnewayLines(horizontal(r), vertical(r)) &&
	  verticalArrangement(invertedLines)){
	tables.push_back(r);
	tableTree.insert(r);
      }
    }
  }
  return tables;
}
