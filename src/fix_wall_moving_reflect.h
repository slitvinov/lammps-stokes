/* -*- c++ -*- ----------------------------------------------------------
   LAMMPS - Large-scale Atomic/Molecular Massively Parallel Simulator
   http://lammps.sandia.gov, Sandia National Laboratories
   Steve Plimpton, sjplimp@sandia.gov

   Copyright (2003) Sandia Corporation.  Under the terms of Contract
   DE-AC04-94AL85000 with Sandia Corporation, the U.S. Government retains
   certain rights in this software.  This software is distributed under
   the GNU General Public License.

   See the README file in the top-level LAMMPS directory.
------------------------------------------------------------------------- */

#ifdef FIX_CLASS

FixStyle(wall/moving/reflect,FixWallMovingReflect)

#else

#ifndef LMP_FIX_WALL_MOVING_REFLECT_H
#define LMP_FIX_WALL_MOVING_REFLECT_H

#include "fix.h"

namespace LAMMPS_NS {

class FixWallMovingReflect : public Fix {
 public:
  FixWallMovingReflect(class LAMMPS *, int, char **);
  virtual ~FixWallMovingReflect();
  int setmask();
  void init();
  void post_integrate();

 protected:
  int nwall;
  int wallwhich[6],wallstyle[6];
  double coord0[6];
  char *varstr[6];
  int varindex[6];
  int varflag;
  double xscale,yscale,zscale;
  double v0[3]; /* velocities of the walls */
  double v1[3];
};

}

#endif
#endif

/* ERROR/WARNING messages:

E: Illegal ... command

Self-explanatory.  Check the input script syntax and compare to the
documentation for the command.  You can use -echo screen as a
command-line option when running LAMMPS to see the offending line.

E: Wall defined twice in fix wall/moving/reflect command

Self-explanatory.

E: Cannot use fix wall/moving/reflect in periodic dimension

Self-explanatory.

E: Cannot use fix wall/moving/reflect zlo/zhi for a 2d simulation

Self-explanatory.

E: Variable name for fix wall/moving/reflect does not exist

Self-explanatory.

E: Variable for fix wall/moving/reflect is invalid style

Only equal-style variables can be used.

W: Should not allow rigid bodies to bounce off relecting walls

LAMMPS allows this, but their dynamics are not computed correctly.

*/
