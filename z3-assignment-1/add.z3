(declare-const a (_ BitVec 4))
(declare-const b (_ BitVec 4))
(declare-const d (_ BitVec 4))


(declare-const a0 (_ BitVec 1))
(declare-const a1 (_ BitVec 1))
(declare-const a2 (_ BitVec 1))
(declare-const a3 (_ BitVec 1))

(declare-const b0 (_ BitVec 1))
(declare-const b1 (_ BitVec 1))
(declare-const b2 (_ BitVec 1))
(declare-const b3 (_ BitVec 1))

(declare-const d0 (_ BitVec 1))
(declare-const d1 (_ BitVec 1))
(declare-const d2 (_ BitVec 1))
(declare-const d3 (_ BitVec 1))

(declare-const c0 (_ BitVec 1))
(declare-const c1 (_ BitVec 1))
(declare-const c2 (_ BitVec 1))
(declare-const c3 (_ BitVec 1))
(declare-const c4 (_ BitVec 1))


(assert (= d (bvadd a b)))

(assert (= a0 ((_ extract 0 0) a)))
(assert (= a1 ((_ extract 1 1) a)))
(assert (= a2 ((_ extract 2 2) a)))
(assert (= a3 ((_ extract 3 3) a)))

(assert (= b0 ((_ extract 0 0) b)))
(assert (= b1 ((_ extract 1 1) b)))
(assert (= b2 ((_ extract 2 2) b)))
(assert (= b3 ((_ extract 3 3) b)))

(assert (= c0 #b0))

(assert (= d0 (bvxor (bvxor a0 b0) c0)))
(assert (= d1 (bvxor (bvxor a1 b1) c1)))
(assert (= d2 (bvxor (bvxor a2 b2) c2)))
(assert (= d3 (bvxor (bvxor a3 b3) c3)))

(assert (= c1 (bvor (bvand a0 b0)  (bvand (bvxor a0 b0) c0)) ))
(assert (= c2 (bvor (bvand a1 b1)  (bvand (bvor a1 b1) c1)) ))
(assert (= c3 (bvor (bvand a2 b2)  (bvand (bvor a2 b2) c2)) ))
(assert (= c4 (bvor (bvand a3 b3)  (bvand (bvxor a3 b3) c3)) ))

(assert (or (not (= d0 ((_ extract 0 0) d))) 
            (not (= d1 ((_ extract 1 1) d))) 
            (not (= d2 ((_ extract 2 2) d))) 
            (not (= d3 ((_ extract 3 3) d))) 
				))

(check-sat)
(get-model)
