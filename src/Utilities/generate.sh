#!/bin/bash
# generate file with particle information 
# will generate "n" particles, with id "pid" for each momentum and angle given 
if [ -z "$1" ] ; then
  echo "need number of particles to be generated!"
  exit 1
fi

if [ -z "$2" ] ; then
  echo "need particle Id! (CORSIKA)"
  exit 1
fi


n="$1"
pid="$2"

awk '
BEGIN {
  pi = 4*atan2(1, 1)
  rad = pi / 180
  GeV = 1 / 1000
  n = '$n'
  pid = '$pid'
  #momentum = "100 200 300 400 500 600 700 800 900 1000"
  momentum = "1" # in GeV
  split(momentum, p, " ")
  for (pi = 1; pi <= length(p); ++pi) {
    angles = "0"
    split(angles, a, " ")
    for (ai = 1; ai <= length(a); ++ai) {
      th = a[ai]*rad
      phi = rand()*2*pi
      px = p[pi]*sin(th)*cos(phi)
      py = p[pi]*sin(th)*sin(phi)
      pz = p[pi]*cos(th)
      for (i = 0; i < n; ++i)
      print pid, px, py, pz, 0, 0, 0, 0, 0, 0, 0, 0
    }
  }
}
'
