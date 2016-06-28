# Install/unInstall package files in LAMMPS
# mode = 0/1/2 for uninstall/install/update

mode=$1

# arg1 = file, arg2 = file it depends on

action () {
  if (test $mode = 0) then
    rm -f ../$1
  elif (! cmp -s $1 ../$1) then
    if (test -z "$2" || test -e ../$2) then
      cp $1 ..
      if (test $mode = 2) then
        echo "  updating src/$1"
      fi
    fi
  elif (test -n "$2") then
    if (test ! -e ../$2) then
      rm -f ../$1
    fi
  fi
}

# step 1: process all *_intel.cpp and *_intel.h files.
# do not install child files if parent does not exist

for file in *_intel.cpp; do
  dep=`echo $file | sed 's/neigh_full_intel/neigh_full/g' | \
      sed 's/_offload_intel//g' | sed 's/_intel//g'`
  action $file $dep
done

for file in *_intel.h; do
  dep=`echo $file | sed 's/_offload_intel//g' | sed 's/_intel//g'`
  action $file $dep
done

action intel_preprocess.h
action intel_buffers.h
action intel_buffers.cpp
action math_extra_intel.h
action intel_simd.h pair_sw_intel.cpp
action intel_intrinsics.h pair_tersoff_intel.cpp
action verlet_lrt_intel.h pppm.cpp
action verlet_lrt_intel.cpp pppm.cpp

# step 2: handle cases and tasks not handled in step 1.

if (test $mode = 1) then

  if (test -e ../Makefile.package) then
    sed -i -e 's/[^ \t]*INTEL[^ \t]* //' ../Makefile.package
    sed -i -e 's|^PKG_INC =[ \t]*|&-DLMP_USER_INTEL |' ../Makefile.package
  fi

  # force rebuild of files with LMP_USER_INTEL switch

  touch ../accelerator_intel.h

elif (test $mode = 0) then

  if (test -e ../Makefile.package) then
    sed -i -e 's/[^ \t]*INTEL[^ \t]* //' ../Makefile.package
  fi

  # force rebuild of files with LMP_USER_INTEL switch

  touch ../accelerator_intel.h

fi
