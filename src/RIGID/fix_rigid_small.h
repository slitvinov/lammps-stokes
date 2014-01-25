/* ----------------------------------------------------------------------
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

FixStyle(rigid/small,FixRigidSmall)

#else

#ifndef LMP_FIX_RIGID_SMALL_H
#define LMP_FIX_RIGID_SMALL_H

#include "fix.h"

// replace this later
#include <map>

namespace LAMMPS_NS {

class FixRigidSmall : public Fix {
 public:
  // static variable for ring communication callback to access class data

  static FixRigidSmall *frsptr;

  FixRigidSmall(class LAMMPS *, int, char **);
  virtual ~FixRigidSmall();
  virtual int setmask();
  virtual void init();
  virtual void setup(int);
  virtual void initial_integrate(int);
  void post_force(int);
  virtual void final_integrate();
  void initial_integrate_respa(int, int, int);
  void final_integrate_respa(int, int);
  void write_restart_file(char *);

  void grow_arrays(int);
  void copy_arrays(int, int, int);
  void set_arrays(int);
  void set_molecule(int, tagint, double *, double *, double *);

  int pack_exchange(int, double *);
  int unpack_exchange(int, double *);
  int pack_comm(int, int *, double *, int, int *);
  void unpack_comm(int, int, double *);
  int pack_reverse_comm(int, int, double *);
  void unpack_reverse_comm(int, int *, double *);

  void setup_pre_neighbor();
  void pre_neighbor();
  int dof(int);
  void deform(int);
  void reset_dt();
  void zero_momentum();
  void zero_rotation();
  void *extract(const char*, int &);
  double extract_ke();
  double extract_erotational();
  double compute_scalar();
  double memory_usage();

 protected:
  int me,nprocs;
  double dtv,dtf,dtq;
  double *step_respa;
  int triclinic;
  double MINUSPI,TWOPI;

  char *infile;             // file to read rigid body attributes from
  int firstflag;            // 1 for first-time setup of rigid bodies
  int commflag;             // various modes of forward/reverse comm
  int nbody;                // total # of rigid bodies
  tagint maxmol;            // max mol-ID
  double maxextent;         // furthest distance from body owner to body atom

  struct Body {
    double mass;              // total mass of body
    double xcm[3];            // COM position
    double vcm[3];            // COM velocity
    double fcm[3];            // force on COM
    double torque[3];         // torque around COM
    double quat[4];           // quaternion for orientation of body
    double inertia[3];        // 3 principal components of inertia
    double ex_space[3];       // principal axes in space coords
    double ey_space[3];
    double ez_space[3];
    double angmom[3];         // space-frame angular momentum of body
    double omega[3];          // space-frame omega of body
    imageint image;             // image flags of xcm
    int remapflag[4];         // PBC remap flags
    int ilocal;               // index of owning atom
  };

  Body *body;               // list of rigid bodies, owned and ghost
  int nlocal_body;          // # of owned rigid bodies
  int nghost_body;          // # of ghost rigid bodies
  int nmax_body;            // max # of bodies that body can hold
  int bodysize;             // sizeof(Body) in doubles

  // per-atom quantities
  // only defined for owned atoms, except bodyown for own+ghost

  int *bodyown;         // index of body if atom owns a body, -1 if not
  tagint *bodytag;      // ID of body this atom is in, 0 if none
                        // ID = tag of atom that owns body
  int *atom2body;       // index of owned/ghost body this atom is in, -1 if not
                        // can point to original or any image of the body
  double **displace;    // displacement of each atom in body coords

  int *eflags;              // flags for extended particles
  double **orient;          // orientation vector of particle wrt rigid body
  double **dorient;         // orientation of dipole mu wrt rigid body

  int extended;             // 1 if any particles have extended attributes
  int orientflag;           // 1 if particles store spatial orientation
  int dorientflag;          // 1 if particles store dipole orientation

  int POINT,SPHERE,ELLIPSOID,LINE,TRIANGLE,DIPOLE;   // bitmasks for eflags
  int OMEGA,ANGMOM,TORQUE;

  class AtomVecEllipsoid *avec_ellipsoid;
  class AtomVecLine *avec_line;
  class AtomVecTri *avec_tri;

  // temporary per-body storage

  int **counts;            // counts of atom types in bodies
  double **itensor;        // 6 space-frame components of inertia tensor

  // mass per body, accessed by granular pair styles

  double *mass_body;
  int nmax_mass;

  // Langevin thermostatting

  int langflag;                     // 0/1 = no/yes Langevin thermostat
  double t_start,t_stop,t_period;   // thermostat params
  double **langextra;               // Langevin thermostat forces and torques
  int maxlang;                      // max size of langextra
  class RanMars *random;            // RNG

  // molecules added on-the-fly as rigid bodies

  class Molecule *onemol;

  // class data used by ring communication callbacks

  std::map<tagint,int> *hash;
  double **bbox;
  double **ctr;
  tagint *idclose;
  double *rsqclose;
  double rsqfar;

  void set_xv();
  void set_v();
  void create_bodies();
  void setup_bodies_static();
  void setup_bodies_dynamic();
  void readfile(int, double **, int *);
  void grow_body();
  void reset_atom2body();

  // callback functions for ring communication

  static void ring_bbox(int, char *);
  static void ring_nearest(int, char *);
  static void ring_farthest(int, char *);

  // debug

  //void check(int);
};

}

#endif
#endif

/* ERROR/WARNING messages:

E: Illegal ... command

Self-explanatory.  Check the input script syntax and compare to the
documentation for the command.  You can use -echo screen as a
command-line option when running LAMMPS to see the offending line.

E: Fix rigid/small langevin period must be > 0.0

Self-explanatory.

E: Fix rigid/small requires atom attribute molecule

Self-explanatory.

E: No rigid bodies defined

The fix specification did not end up defining any rigid bodies.

W: More than one fix rigid

It is not efficient to use fix rigid more than once.

E: Rigid fix must come before NPT/NPH fix

NPT/NPH fix must be defined in input script after all rigid fixes,
else the rigid fix contribution to the pressure virial is
incorrect.

W: Cannot count rigid body degrees-of-freedom before bodies are fully initialized

UNDOCUMENTED

W: Computing temperature of portions of rigid bodies

The group defined by the temperature compute does not encompass all
the atoms in one or more rigid bodies, so the change in
degrees-of-freedom for the atoms in those partial rigid bodies will
not be accounted for.

E: Fix rigid/small atom has non-zero image flag in a non-periodic dimension

Image flags for non-periodic dimensions should not be set.

E: Insufficient Jacobi rotations for rigid body

Eigensolve for rigid body was not sufficiently accurate.

E: Fix rigid: Bad principal moments

The principal moments of inertia computed for a rigid body
are not within the required tolerances.

E: Rigid body atoms %d %d missing on proc %d at step %ld

This means that an atom cannot find the atom that owns the rigid body
it is part of, or vice versa.  The solution is to use the communicate
cutoff command to insure ghost atoms are acquired from far enough away
to encompass the max distance printed when the fix rigid/small command
was invoked.

*/
