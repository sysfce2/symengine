#include <symengine/logic.h>

namespace SymEngine
{

BooleanAtom::BooleanAtom(bool b) : b_{b}
{
    SYMENGINE_ASSIGN_TYPEID()
}

hash_t BooleanAtom::__hash__() const
{
    hash_t seed = BOOLEAN_ATOM;
    if (b_)
        ++seed;
    return seed;
}
bool BooleanAtom::get_val() const
{
    return b_;
}

vec_basic BooleanAtom::get_args() const
{
    return {};
}

bool BooleanAtom::__eq__(const Basic &o) const
{
    return is_a<BooleanAtom>(o)
           and get_val() == down_cast<const BooleanAtom &>(o).get_val();
}

int BooleanAtom::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<BooleanAtom>(o))
    bool ob = down_cast<const BooleanAtom &>(o).get_val();
    if (get_val()) {
        return (ob) ? 0 : 1;
    } else {
        return (ob) ? -1 : 0;
    }
}

RCP<const BooleanAtom> boolTrue = make_rcp<BooleanAtom>(true);
RCP<const BooleanAtom> boolFalse = make_rcp<BooleanAtom>(false);

Contains::Contains(const RCP<const Basic> &expr, const RCP<const Set> &set)
    : expr_{expr}, set_{set}
{
    SYMENGINE_ASSIGN_TYPEID()
}

hash_t Contains::__hash__() const
{
    hash_t seed = CONTAINS;
    hash_combine<Basic>(seed, *expr_);
    hash_combine<Basic>(seed, *set_);
    return seed;
}

RCP<const Basic> Contains::get_expr() const
{
    return expr_;
}

RCP<const Set> Contains::get_set() const
{
    return set_;
}

vec_basic Contains::get_args() const
{
    vec_basic v;
    v.push_back(expr_);
    v.push_back(set_);
    return v;
}

bool Contains::__eq__(const Basic &o) const
{
    return is_a<Contains>(o)
           and unified_eq(get_expr(), down_cast<const Contains &>(o).get_expr())
           and unified_eq(get_set(), down_cast<const Contains &>(o).get_set());
}

int Contains::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Contains>(o))
    const Contains &c = down_cast<const Contains &>(o);
    int cmp = unified_compare(get_expr(), c.get_expr());
    if (cmp != 0)
        return cmp;
    return unified_compare(get_set(), c.get_set());
}

RCP<const Boolean> contains(const RCP<const Basic> &expr,
                            const RCP<const Set> &set)
{
    if (is_a_Number(*expr)) {
        return set->contains(expr);
    } else {
        return make_rcp<Contains>(expr, set);
    }
}

Piecewise::Piecewise(PiecewiseVec &&vec) : vec_(vec)
{
    SYMENGINE_ASSIGN_TYPEID()
}

hash_t Piecewise::__hash__() const
{
    hash_t seed = this->get_type_code();
    for (auto &p : vec_) {
        hash_combine<Basic>(seed, *p.first);
        hash_combine<Basic>(seed, *p.second);
    }
    return seed;
}

const PiecewiseVec &Piecewise::get_vec() const
{
    return vec_;
}

vec_basic Piecewise::get_args() const
{
    vec_basic v;
    for (auto &p : vec_) {
        v.push_back(p.first);
        v.push_back(p.second);
    }
    return v;
}

bool Piecewise::__eq__(const Basic &o) const
{
    return is_a<Piecewise>(o)
           and unified_eq(get_vec(), down_cast<const Piecewise &>(o).get_vec());
}

int Piecewise::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_same_type(*this, o))
    RCP<const Piecewise> t = o.rcp_from_this_cast<Piecewise>();
    return unified_compare(get_vec(), t->get_vec());
}

And::And(const set_boolean &s) : container_{s}
{
    SYMENGINE_ASSIGN_TYPEID()
    SYMENGINE_ASSERT(is_canonical(s));
}

hash_t And::__hash__() const
{
    hash_t seed = AND;
    for (const auto &a : container_)
        hash_combine<Basic>(seed, *a);
    return seed;
}

vec_basic And::get_args() const
{
    vec_basic v(container_.begin(), container_.end());
    return v;
}

bool And::__eq__(const Basic &o) const
{
    return is_a<And>(o) and unified_eq(container_, down_cast<const And &>(o)
                                                       .get_container());
}

int And::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<And>(o))
    return unified_compare(container_,
                           down_cast<const And &>(o).get_container());
}

bool And::is_canonical(const set_boolean &container_)
{
    if (container_.size() >= 2) {
        for (auto &a : container_) {
            if (is_a<BooleanAtom>(*a) or is_a<And>(*a))
                return false;
            if (container_.find(logical_not(a)) != container_.end())
                return false;
        }
        return true;
    }
    return false;
}

const set_boolean &And::get_container() const
{
    return container_;
}

Or::Or(const set_boolean &s) : container_{s}
{
    SYMENGINE_ASSIGN_TYPEID()
    SYMENGINE_ASSERT(is_canonical(s));
}

hash_t Or::__hash__() const
{
    hash_t seed = OR;
    for (const auto &a : container_)
        hash_combine<Basic>(seed, *a);
    return seed;
}

vec_basic Or::get_args() const
{
    vec_basic v(container_.begin(), container_.end());
    return v;
}

bool Or::__eq__(const Basic &o) const
{
    return is_a<Or>(o)
           and unified_eq(container_, down_cast<const Or &>(o).get_container());
}

int Or::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Or>(o))
    return unified_compare(container_,
                           down_cast<const Or &>(o).get_container());
}

bool Or::is_canonical(const set_boolean &container_)
{
    if (container_.size() >= 2) {
        for (auto &a : container_) {
            if (is_a<BooleanAtom>(*a) or is_a<Or>(*a))
                return false;
            if (container_.find(logical_not(a)) != container_.end())
                return false;
        }
        return true;
    }
    return false;
}

const set_boolean &Or::get_container() const
{
    return container_;
}

Not::Not(const RCP<const Boolean> &in) : arg_{in}
{
    SYMENGINE_ASSIGN_TYPEID()
    SYMENGINE_ASSERT(is_canonical(in));
}

hash_t Not::__hash__() const
{
    hash_t seed = NOT;
    hash_combine<Basic>(seed, *arg_);
    return seed;
}

vec_basic Not::get_args() const
{
    vec_basic v;
    v.push_back(arg_);
    return v;
}

bool Not::__eq__(const Basic &o) const
{
    return is_a<Not>(o) and eq(*arg_, *down_cast<const Not &>(o).get_arg());
}

int Not::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Not>(o))
    return arg_->__cmp__(*down_cast<const Not &>(o).get_arg());
}

bool Not::is_canonical(const RCP<const Boolean> &in)
{
    if (is_a<BooleanAtom>(*in) or is_a<Not>(*in))
        return false;
    return true;
}

RCP<const Boolean> Not::get_arg() const
{
    return arg_;
}

Xor::Xor(const vec_boolean &s) : container_{s}
{
    SYMENGINE_ASSIGN_TYPEID()
    SYMENGINE_ASSERT(is_canonical(s));
}

hash_t Xor::__hash__() const
{
    hash_t seed = XOR;
    for (const auto &a : container_)
        hash_combine<Basic>(seed, *a);
    return seed;
}

vec_basic Xor::get_args() const
{
    vec_basic v(container_.begin(), container_.end());
    return v;
}

bool Xor::__eq__(const Basic &o) const
{
    return is_a<Xor>(o) and unified_eq(container_, down_cast<const Xor &>(o)
                                                       .get_container());
}

int Xor::compare(const Basic &o) const
{
    SYMENGINE_ASSERT(is_a<Xor>(o))
    return unified_compare(container_,
                           down_cast<const Xor &>(o).get_container());
}

bool Xor::is_canonical(const vec_boolean &container_)
{
    if (container_.size() >= 2) {
        set_boolean args;
        for (auto &a : container_) {
            if (is_a<BooleanAtom>(*a) or is_a<Xor>(*a)) {
                return false;
            }
            if (args.find(a) != args.end()) {
                return false;
            }
            if (args.find(logical_not(a)) != args.end()) {
                return false;
            }
            args.insert(a);
        }
        return true;
    }
    return false;
}

const vec_boolean &Xor::get_container() const
{
    return container_;
}

const vec_boolean get_vec_from_set(const set_boolean &s)
{
    vec_boolean v(s.begin(), s.end());
    return v;
}

template <typename caller>
RCP<const Boolean> and_or(const set_boolean &s, const bool &op_x_notx)
{
    set_boolean args;
    for (auto &a : s) {
        if (is_a<BooleanAtom>(*a)) {
            auto val = down_cast<const BooleanAtom &>(*a).get_val();
            if (val == op_x_notx)
                return boolean(op_x_notx);
            else
                continue;
        }
        if (is_a<caller>(*a)) {
            const caller &to_insert = down_cast<const caller &>(*a);
            auto container = to_insert.get_container();
            args.insert(container.begin(), container.end());
            continue;
        }
        args.insert(a);
    }
    for (auto &a : args) {
        if (args.find(logical_not(a)) != args.end())
            return boolean(op_x_notx);
    }
    if (args.size() == 1)
        return *(args.begin());
    else if (args.size() == 0)
        return boolean(not op_x_notx);
    return make_rcp<const caller>(args);
}

RCP<const Boolean> logical_not(const RCP<const Boolean> &s)
{
    if (is_a<BooleanAtom>(*s)) {
        const BooleanAtom &a = down_cast<const BooleanAtom &>(*s);
        return boolean(not a.get_val());
    } else if (is_a<Not>(*s)) {
        const Not &a = down_cast<const Not &>(*s);
        return a.get_arg();
    } else if (is_a<Or>(*s)) {
        auto container = down_cast<const Or &>(*s).get_container();
        set_boolean cont;
        for (auto &a : container) {
            cont.insert(logical_not(a));
        }
        return make_rcp<const And>(cont);
    } else if (is_a<And>(*s)) {
        auto container = down_cast<const And &>(*s).get_container();
        set_boolean cont;
        for (auto &a : container) {
            cont.insert(logical_not(a));
        }
        return make_rcp<const Or>(cont);
    } else {
        return make_rcp<const Not>(s);
    }
}

RCP<const Boolean> logical_xor(const vec_boolean &s)
{
    set_boolean args;
    int nots = 0;
    for (auto &a : s) {
        if (is_a<BooleanAtom>(*a)) {
            auto val = down_cast<const BooleanAtom &>(*a).get_val();
            if (val == true) {
                nots++;
            }
            continue;
        } else if (is_a<Xor>(*a)) {
            auto container = down_cast<const Xor &>(*a).get_container();
            for (auto &aa : container) {
                if (args.find(aa) != args.end()) {
                    args.erase(aa);
                } else {
                    auto pos = args.find(logical_not(aa));
                    if (pos != args.end()) {
                        args.erase(pos);
                        nots++;
                    } else {
                        args.insert(aa);
                    }
                }
            }
            continue;
        }
        if (args.find(a) != args.end()) {
            args.erase(a);
        } else {
            auto pos = args.find(logical_not(a));
            if (pos != args.end()) {
                args.erase(pos);
                nots++;
            } else {
                args.insert(a);
            }
        }
    }

    if (nots % 2 == 0) {
        if (args.size() == 0) {
            return boolFalse;
        } else if (args.size() == 1) {
            return *args.begin();
        } else {
            return make_rcp<const Xor>(get_vec_from_set(args));
        }
    } else {
        if (args.size() == 0) {
            return boolTrue;
        } else if (args.size() == 1) {
            return logical_not(*args.begin());
        } else {
            return make_rcp<const Not>(
                make_rcp<const Xor>(get_vec_from_set(args)));
        }
    }
}

Relational::Relational(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
    : TwoArgBasic<Boolean>(lhs, rhs)
{
}

inline bool Relational::is_canonical(const RCP<const Basic> &lhs,
                                     const RCP<const Basic> &rhs) const
{
    if (eq(*lhs, *rhs))
        return false;
    if (is_a_Number(*lhs) and is_a_Number(*rhs))
        return false;
    if (is_a<BooleanAtom>(*lhs) and is_a<BooleanAtom>(*rhs))
        return false;
    return true;
}

Equality::Equality(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
    : Relational(lhs, rhs)
{
    SYMENGINE_ASSIGN_TYPEID();
    SYMENGINE_ASSERT(is_canonical(lhs, rhs));
}

RCP<const Basic> Equality::create(const RCP<const Basic> &lhs,
                                  const RCP<const Basic> &rhs) const
{
    return Eq(lhs, rhs);
}

RCP<const Basic> Eq(const RCP<const Basic> &lhs)
{
    return Eq(lhs, zero);
}

RCP<const Basic> Eq(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
{
    if (is_a<NaN>(*lhs) or is_a<NaN>(*rhs))
        return boolean(false);
    bool b = eq(*lhs, *rhs);
    if (b) {
        return boolean(true);
    } else {
        if ((is_a_Number(*lhs) and is_a_Number(*rhs))
            or (is_a<BooleanAtom>(*lhs) and is_a<BooleanAtom>(*rhs)))
            return boolean(false);
        if (lhs->__cmp__(*rhs) == 1)
            return make_rcp<const Equality>(rhs, lhs);
        return make_rcp<Equality>(lhs, rhs);
    }
}

Unequality::Unequality(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
    : Relational(lhs, rhs)
{
    SYMENGINE_ASSIGN_TYPEID();
    SYMENGINE_ASSERT(is_canonical(lhs, rhs));
}

RCP<const Basic> Unequality::create(const RCP<const Basic> &lhs,
                                    const RCP<const Basic> &rhs) const
{
    return Ne(lhs, rhs);
}

RCP<const Basic> Ne(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
{
    RCP<const Basic> r = Eq(lhs, rhs);
    if (is_a<BooleanAtom>(*r)) {
        return logical_not(rcp_static_cast<const BooleanAtom>(r));
    }
    if (lhs->__cmp__(*rhs) == 1)
        return make_rcp<const Unequality>(rhs, lhs);
    return make_rcp<Unequality>(lhs, rhs);
}

LessThan::LessThan(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
    : Relational(lhs, rhs)
{
    SYMENGINE_ASSIGN_TYPEID();
    SYMENGINE_ASSERT(is_canonical(lhs, rhs));
}

RCP<const Basic> LessThan::create(const RCP<const Basic> &lhs,
                                  const RCP<const Basic> &rhs) const
{
    return Le(lhs, rhs);
}

RCP<const Basic> Le(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
{
    if (is_a_Complex(*lhs) or is_a_Complex(*rhs))
        throw SymEngineException("Invalid comparison of complex numbers.");
    if (is_a<NaN>(*lhs) or is_a<NaN>(*rhs))
        throw SymEngineException("Invalid NaN comparison.");
    if (eq(*lhs, *ComplexInf) or eq(*rhs, *ComplexInf))
        throw SymEngineException("Invalid comparison of complex zoo.");
    if (is_a<BooleanAtom>(*lhs) or is_a<BooleanAtom>(*rhs))
        throw SymEngineException("Invalid comparison of Boolean objects.");
    if (eq(*lhs, *rhs))
        return boolean(true);
    if (is_a_Number(*lhs) and is_a_Number(*rhs)) {
        RCP<const Number> s = down_cast<const Number &>(*lhs)
                                  .sub(down_cast<const Number &>(*rhs));
        if (s->is_negative())
            return boolean(true);
        return boolean(false);
    }
    return make_rcp<const LessThan>(lhs, rhs);
}

RCP<const Basic> Ge(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
{
    return Le(rhs, lhs);
}

StrictLessThan::StrictLessThan(const RCP<const Basic> &lhs,
                               const RCP<const Basic> &rhs)
    : Relational(lhs, rhs)
{
    SYMENGINE_ASSIGN_TYPEID();
    SYMENGINE_ASSERT(is_canonical(lhs, rhs));
}

RCP<const Basic> StrictLessThan::create(const RCP<const Basic> &lhs,
                                        const RCP<const Basic> &rhs) const
{
    return Lt(lhs, rhs);
}

RCP<const Basic> Lt(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
{
    if (is_a_Complex(*lhs) or is_a_Complex(*rhs))
        throw SymEngineException("Invalid comparison of complex numbers.");
    if (is_a<NaN>(*lhs) or is_a<NaN>(*rhs))
        throw SymEngineException("Invalid NaN comparison.");
    if (eq(*lhs, *ComplexInf) or eq(*rhs, *ComplexInf))
        throw SymEngineException("Invalid comparison of complex zoo.");
    if (is_a<BooleanAtom>(*lhs) or is_a<BooleanAtom>(*rhs))
        throw SymEngineException("Invalid comparison of Boolean objects.");
    if (eq(*lhs, *rhs))
        return boolean(false);
    if (is_a_Number(*lhs) and is_a_Number(*rhs)) {
        RCP<const Number> s = down_cast<const Number &>(*lhs)
                                  .sub(down_cast<const Number &>(*rhs));
        if (s->is_negative())
            return boolean(true);
        return boolean(false);
    }
    return make_rcp<const StrictLessThan>(lhs, rhs);
}

RCP<const Basic> Gt(const RCP<const Basic> &lhs, const RCP<const Basic> &rhs)
{
    return Lt(rhs, lhs);
}

RCP<const Boolean> logical_and(const set_boolean &s)
{
    return and_or<And>(s, false);
}

RCP<const Boolean> logical_nand(const set_boolean &s)
{
    RCP<const Boolean> a = logical_and(s);
    return logical_not(a);
}

RCP<const Boolean> logical_or(const set_boolean &s)
{
    return and_or<Or>(s, true);
}

RCP<const Boolean> logical_nor(const set_boolean &s)
{
    return logical_not(and_or<Or>(s, true));
}

RCP<const Boolean> logical_xnor(const vec_boolean &s)
{
    return logical_not(logical_xor(s));
}
}
