/*
  -------------------------------------------------------------------
  
  Copyright (C) 2015, Andrew W. Steiner
  
  This file is part of O2scl.
  
  O2scl is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 3 of the License, or
  (at your option) any later version.
  
  O2scl is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with O2scl. If not, see <http://www.gnu.org/licenses/>.

  -------------------------------------------------------------------
*/
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <o2scl/test_mgr.h>
#include <o2scl/tov_love.h>
#include <o2scl/tov_solve.h>
#include <o2scl/eos_had_apr.h>
#include <o2scl/nstar_cold.h>

using namespace std;
using namespace o2scl;
using namespace o2scl_const;

int main(void) {

  cout.setf(ios::scientific);

  test_mgr t;
  t.set_output_level(1);

  double schwarz_km=o2scl_cgs::schwarzchild_radius/1.0e5;
  
  eos_had_apr apr;
  apr.pion=1;
  
  nstar_cold nst;
  nst.set_eos(apr);
  nst.calc_eos();
  
  o2_shared_ptr<table_units<> >::type te=nst.get_eos_results();

  eos_tov_interp eti;
  eti.default_low_dens_eos();
  eti.read_table(*te,"ed","pr","nb");

  tov_solve ts;
  ts.verbose=0;
  ts.set_eos(eti);
  ts.calc_gpot=true;
  ts.ang_vel=true;
  ts.fixed(1.4);
  
  o2_shared_ptr<table_units<> >::type profile=ts.get_results();

  // I'm not sure why cs2 isn't computed properly, so
  // we have to recompute it here
  profile->delete_column("cs2");
  profile->deriv("ed","pr","cs2");
  
  // Moment of inertia
  double I=ts.domega_rat*pow(ts.rad,4.0)/3.0/schwarz_km;
  cout << "Radius: " << ts.rad << " km" << endl;
  cout << "Moment of inertia: " << I << " Msun*km^2" << endl;
  double Ibar=I/pow(schwarz_km/2.0,2.0)/pow(1.4,3.0);
  cout << "Dimensionless moment of inertia: "
       << Ibar << endl;
    

  // Tidal deformability
  tov_love tl;
  tl.tab=profile;
  double yR, beta, k2, lambda_km5, lambda_cgs, lbar;
  
  tl.calc_y(yR,beta,k2,lambda_km5,lambda_cgs);
  lbar=lambda_km5/pow(1.4*schwarz_km/2.0,5.0);
  cout << "Dimensionless tidal deformability (direct calculation, y): " 
       << lbar << endl;
  tl.calc_H(yR,beta,k2,lambda_km5,lambda_cgs);
  lbar=lambda_km5/pow(1.4*schwarz_km/2.0,5.0);
  cout << "Dimensionless tidal deformability (direct calculation, H): " 
       << lbar << endl;

  // Compute the Correlation
  double l_lbar=log(lbar);
  double l_Ibar=1.47+0.0817*l_lbar+0.0149*l_lbar*l_lbar+
    2.87e-4*l_lbar*l_lbar*l_lbar-3.64e-5*l_lbar*l_lbar*l_lbar*l_lbar;
  cout << "Dimensionless moment of inertia from YY13 correlation: "
       << exp(l_Ibar) << endl;
  
  double acc=fabs(exp(l_Ibar)-Ibar)/Ibar;
  cout << "Relative deviation: " << acc << endl;
  t.test_abs(acc,0.0,0.015,"lambda and I");
  
  t.report();
  
  return 0;
}

