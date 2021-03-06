#include "heuristic_filter.hpp"
#include <vector>
#include <boost/geometry.hpp>
#include "utility.hpp"
#include "component_stats.hpp"
#include "image_util.hpp"

using namespace std;
using namespace cv;
namespace bgi = boost::geometry::index;

void heuristic_filter(Mat& text, Mat& nontext){
  remove_lines(text, nontext);
  Mat cc;
  vector<ComponentStats> stats = statistics(text, cc);
  bgi::rtree<ComponentStats, bgi::quadratic<16> > tree(stats);
  for (ComponentStats& cs : stats){
    if (cs.area < 6)
      bitwise_not(text, text, cc == cs.index);
    else if ((cs.hwratio < 0.15 && cs.r.width > cs.r.height) ||
	cs.density < 0.25 ||
	distance(tree.qbegin(bgi::intersects(cs)), tree.qend()) > 4){
      move2(text, nontext, cc, cs.index);
    }
  }
}
