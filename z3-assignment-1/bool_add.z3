(declare-const a (_ BitVec 4))
(declare-const b (_ BitVec 4))
(declare-const d (_ BitVec 4))


(declare-const a0 Bool)
(declare-const a1 Bool)
(declare-const a2 Bool)
(declare-const a3 Bool)

(declare-const b0 Bool)
(declare-const b1 Bool)
(declare-const b2 Bool)
(declare-const b3 Bool)

(declare-const d0 Bool)
(declare-const d1 Bool)
(declare-const d2 Bool)
(declare-const d3 Bool)

(declare-const c0 Bool)
(declare-const c1 Bool)
(declare-const c2 Bool)
(declare-const c3 Bool)
(declare-const c4 Bool)

(assert (= d (bvadd a b)))

(assert (= a0 (bvuge ((_ extract 0 0) a) #b1 ))) ; Extract the bit. Check if it is unsigned greater or equal to 1, then true otherwise false
(assert (= a1 (bvuge ((_ extract 1 1) a) #b1 )))
(assert (= a2 (bvuge ((_ extract 2 2) a) #b1 )))
(assert (= a3 (bvuge ((_ extract 3 3) a) #b1 )))

(assert (= b0 (bvuge ((_ extract 0 0) b) #b1 )))
(assert (= b1 (bvuge ((_ extract 1 1) b) #b1 )))
(assert (= b2 (bvuge ((_ extract 2 2) b) #b1 )))
(assert (= b3 (bvuge ((_ extract 3 3) b) #b1 )))

(assert (= c0 false))

(assert (= d0 (xor (xor a0 b0) c0)))
(assert (= d1 (xor (xor a1 b1) c1)))
(assert (= d2 (xor (xor a2 b2) c2)))
(assert (= d3 (xor (xor a3 b3) c3)))


(assert (= c1 (or (and a0 b0)  (and (xor a0 b0) c0)) ))
(assert (= c2 (or (and a1 b1)  (and (xor a1 b1) c1)) ))
(assert (= c3 (or (and a2 b2)  (and (xor a2 b2) c2)) ))
(assert (= c4 (or (and a3 b3)  (and (xor a3 b3) c3)) ))

(assert (or (not (= d0 (bvuge ((_ extract 0 0) d) #b1))) 
            (not (= d1 (bvuge ((_ extract 1 1) d) #b1))) 
            (not (= d2 (bvuge ((_ extract 2 2) d) #b1)))
            (not (= d3 (bvuge ((_ extract 3 3) d) #b1))) 
				))

(check-sat)
(get-model)
