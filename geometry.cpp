struct pt {
    ll x = 0, y = 0;
    pt() = default;
    pt(ll x, ll y): x(x), y(y) {}
    void read() {
        cin >> x >> y;
    }
    pt operator - (const pt &o) const {
        return pt(x - o.x, y - o.y);
    }
    pt operator + (const pt &o) const {
        return pt(x + o.x, y + o.y);
    }
    ll vector_mul(const pt &o) const {
        return x * o.y - o.x * y;
    }
    ll scalar_mul(const pt &o) const {
        return x * o.x + y * o.y;
    }
};

vector<pt> convex_hull(vector<pt> a) {
    sort(a.begin(), a.end(), [&](pt p1, pt p2) {
        return p1.x < p2.x || (p1.x == p2.x && p1.y < p2.y);
    });
    pt p0 = a[0];
    sort(a.begin() + 1, a.end(), [&](pt p1, pt p2) {
        return (p1 - p0).vector_mul(p2 - p0) > 0;
    });
    vector<pt> st;
    st.push_back(p0);
    for (int i = 1; i < (int)a.size(); i++) {
        pt p = a[i];
        while ((int)st.size() >= 2) {
            pt p1 = st[(int)st.size() - 2];
            pt p2 = st[(int)st.size() - 1];
            if ((p - p2).vector_mul(p1 - p2) > 0) {
                break;
            }
            st.pop_back();
        }
        st.push_back(p);
    }
    return st;
}

// line (a, b)
// segment (c, d)
bool intersect(pt a, pt b, pt c, pt d, ld &xx, ld &yy) {
    ll lx = (b - a).vector_mul(c - a);
    ll rx = (b - a).vector_mul(d - a);
    if ((lx > 0 && rx > 0) || (lx < 0 && rx < 0)) return false;
    if (lx == 0) {
        xx = c.x;
        yy = c.y;
        return true;
    }
    if (rx == 0) {
        xx = d.x;
        yy = d.y;
        return true;
    }
    ll x2 = d.x - c.x;
    ll y2 = d.y - c.y;
    pt v = b - a;
    if (v.x * y2 + x2 * v.y == 0) return false;
    ld k = ld(x2 * (a.y - c.y) - y2 * (a.x - c.x)) / ld(v.x * y2 - x2 * v.y);
    xx = v.x * k + a.x;
    yy = v.y * k + a.y;
    return true;
}

//=================
const ld eps = 1e-9;

struct pt;
struct segment;

struct pt {
    ld x = 0;
    ld y = 0;

    pt operator - (const pt& o) const {
        return {x - o.x, y - o.y};
    }

    pt operator + (const pt& o) const {
        return {x + o.x, y + o.y};
    }

    pt operator * (ld coef) const {
        return {x * coef, y * coef};
    }

    ld vector_mul(const pt& o) const {
        return x * o.y - o.x * y;
    }

    ld scalar_mul(const pt& o) const {
        return x * o.x + y * o.y;
    }

    ld sqr_norm() const {
        return x * x + y * y;
    }

    ld norm() const {
        return sqrtl(sqr_norm());
    }

    ld dist(const pt& o) const {
        return (*this - o).norm();
    }

    ld dist(const segment& s) const;

    bool on_segment(const segment& s) const;
};

struct segment {
    enum TYPE {SEGMENT, RAY, LINE} type = SEGMENT;
    pt a;
    pt b;

    ld dist(const segment& o) const;

    segment to_segment_type() const {
        pt v = b - a;
        if (type == SEGMENT)
            return *this;
        else if (type == RAY)
            return segment{SEGMENT, a, a + v * ld(inf)};
        else if (type == LINE)
            return segment{SEGMENT, a - v * ld(inf), a + v * ld(inf)};
    }

    bool intersects(const segment& o, bool swapped = false) const { // works correctly ONLY for non colinear vectors
        ld vm1 = (b - a).vector_mul(o.a - a);
        ld vm2 = (b - a).vector_mul(o.b - a);
        if ((vm1 > eps && vm2 > eps) || (vm1 < -eps && vm2 < -eps))
            return false;
        return swapped ? true : o.intersects(*this, true);
    }
};

ld pt::dist(const segment &s) const {
    if (s.type == segment::TYPE::LINE)
        return abs((s.a - *this).vector_mul(s.b - *this)) / s.a.dist(s.b);
    if (s.type == segment::TYPE::RAY) {
        if ((s.b - s.a).scalar_mul(*this - s.a) >= -eps)
            return dist(segment{segment::TYPE::LINE, s.a, s.b});
        else
            return dist(s.a);
    }
    if (s.type == segment::TYPE::SEGMENT) {
        if ((s.a - s.b).scalar_mul(*this - s.b) >= -eps)
            return dist(segment{segment::TYPE::RAY, s.a, s.b});
        else
            return dist(s.b);
    }
    assert(false);
}

ld segment::dist(const segment &o) const {
    if (abs((b - a).vector_mul(o.b - o.a)) < eps) {
        return min({a.dist(o), b.dist(o), o.a.dist(*this), o.b.dist(*this)});
    }
    segment s1 = to_segment_type();
    segment s2 = o.to_segment_type();
    return s1.intersects(s2) ? 0 : min({s1.a.dist(s2), s1.b.dist(s2), s2.a.dist(s1), s2.b.dist(s1)});
}