#include <vector>
#include <array>
#include <cstdint>
#include <functional>
#include <algorithm>

using Coord = double;
using Point = std::array<Coord,2>;

// Encode edges of a cell as 0:LEFT,1:TOP,2:RIGHT,3:BOTTOM (clockwise)
enum Edge : uint8_t { L=0, T=1, R=2, B=3 };

// Corner bit order: 1=top-left, 2=top-right, 4=bot-right, 8=bot-left (standard MS)
static inline uint8_t ms_case(bool tl, bool tr, bool br, bool bl){
    return (tl?1:0) | (tr?2:0) | (br?4:0) | (bl?8:0);
}

// Where does an edge sit in world coords for cell (x,y)?
static inline Point edgePoint(int x, int y, Edge e){
    switch (e){
        case L: return { (Coord)x,     (Coord)(y+0.5) };
        case T: return { (Coord)(x+0.5),(Coord)(y+1) };
        case R: return { (Coord)(x+1),  (Coord)(y+0.5) };
        default:/*B*/ return { (Coord)(x+0.5),(Coord)y };
    }
}

// Given a cell case and an entering edge, which edge do we exit from?
// This table picks consistent paths and resolves 5/10 to prefer turning right (CW).
static inline bool nextEdge(uint8_t c, Edge in, Edge& out){
    // -1 means "no edge / not on boundary"
    static const int8_t tbl[16][4] = {
/*0*/  {-1,-1,-1,-1},
/*1*/  { -1,  L,  T, -1},   // tl only -> edges between L<->T
/*2*/  { -1, -1,  R,  T},
/*3*/  { -1,  L,  R,  -1},  // tl,tr -> between L<->R (top band)
/*4*/  {  B, -1, -1,  R},
/*5*/  {  B,  L,  T,  R},   // ambiguous: choose right turn sequence L->T->R->B
/*6*/  {  B, -1,  R,  T},
/*7*/  {  B,  L,  R,  -1},
/*8*/  {  L,  T, -1, -1},
/*9*/  {  L,  T,  -1,  B},
/*10*/ {  L,  T,  R,  B},   // ambiguous: choose right turn sequence
/*11*/ {  L,  T,  R,  -1},
/*12*/ {  -1,  -1, -1,  R},
/*13*/ {  -1,  L,  -1,  R},
/*14*/ {  -1,  -1,  R,  B},
/*15*/ { -1,-1,-1,-1}       // full cell: no boundary
    };
    int8_t e = tbl[c][in];
    if (e < 0) return false;
    out = (Edge)e; return true;
}

// Move to neighbor cell when crossing an edge
static inline void stepCell(int& x, int& y, Edge e){
    switch (e){
        case L: --x; break;
        case R: ++x; break;
        case T: ++y; break;
        case B: --y; break;
    }
}

// Find first boundary cell+edge to start tracing
static bool findStart(
    int W,int H,
    const std::function<bool(int,int)>& filled,
    int& sx,int& sy, Edge& se)
{
    for(int y=0;y<H-1;++y){
        for(int x=0;x<W-1;++x){
            bool tl=filled(x,y), tr=filled(x+1,y),
                 br=filled(x+1,y+1), bl=filled(x,y+1);
            uint8_t c = ms_case(tl,tr,br,bl);
            if (c==0 || c==15) continue;
            // Prefer starting on LEFT edge if it's part of the contour; else any
            for (Edge e : {L,T,R,B}){
                Edge out;
                if (nextEdge(c, e, out)) { sx=x; sy=y; se=e; return true; }
            }
        }
    }
    return false;
}

// Trace one closed ring (CW with the table above)
std::vector<Point> traceContour(
    int W,int H,
    const std::function<bool(int,int)>& filled)
{
    int x,y; Edge e;
    if (!findStart(W,H,filled,x,y,e)) return {};

    const int startX = x, startY = y; const Edge startE = e;
    std::vector<Point> ring; ring.reserve(256);

    // Standard marching: at each step, from (x,y) entering via edge e,
    // compute case, get out edge, move to neighbor cell, new entering edge is opposite.
    auto opposite = [](Edge ed)->Edge{ return (Edge)((ed+2)&3); };

    int guard= W*H*8; // safety
    do{
        bool tl=filled(x,y), tr=filled(x+1,y),
             br=filled(x+1,y+1), bl=filled(x,y+1);
        uint8_t c = ms_case(tl,tr,br,bl);
        Edge out;
        if (!nextEdge(c, e, out)) break; // reached a dead end (data hole)

        // Emit vertex on the *boundary* (midpoint of the side)
        ring.push_back(edgePoint(x,y,out));

        // Step across that edge into neighbor cell
        stepCell(x,y,out);
        e = opposite(out);
    } while(--guard && !(x==startX && y==startY && e==startE));

    // Optional: dedupe final repeated point
    if (!ring.empty() && ring.front()==ring.back()) ring.pop_back();
    return ring;
}
