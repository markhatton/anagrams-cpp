#!/bin/bash

STATUS=0

function fail() {
	echo "failure: $1" >&2
	STATUS=1
}

expected='a b c
a bc
a c b
ab c
b a c
b c a
bc a
c a b
c ab
c b a'
solutions=$(./anagrams abc -u <(printf 'a\t1\nb\t1\nc\t1\nab\t1\nbc\t1\n'))
[[ "$solutions" == "$expected" ]] || fail "[$solutions] != [$expected]"


exit $STATUS
