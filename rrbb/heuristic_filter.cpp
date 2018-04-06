#include "heuristic_filter.hpp"
#include <vector>
#include <boost/geometry.hpp>
#include <boost/geometry/core/cs.hpp>
#include <boost/geometry/geometries/register/point.hpp>
#include <boost/geometry/geometries/register/box.hpp>
#include "utility.hpp"
#include "image_util.hpp"

using namespace std;
using namespace cv;
namespace bgi = boost::geometry::index;

BOOST_GEOMETRY_REGISTER_POINT_2D(Point, int, cs::cartesian, x, y)
BOOST_GEOMETRY_REGISTER_BOX(ComponentStats, Point, r.tl(), r.br())

void heuristic_filter(Mat& text, Mat& nontext){
  remove_lines(text, nontext);
  Mat cc;
  vector<ComponentStats> stats = statistics(text, cc);
  bgi::rtree<ComponentStats, bgi::quadratic<16> > tree(stats);						       
  for (ComponentStats& cs : stats){
    if (cs.area < 6 ||
	(cs.hwratio < 0.15 && cs.r.width > cs.r.height) ||      
	cs.density < 0.15 ||
	distance(tree.qbegin(bgi::intersects(cs)), tree.qend()) > 5){
      move2(text, nontext, cc, cs.index);
    }    
  }  
}
