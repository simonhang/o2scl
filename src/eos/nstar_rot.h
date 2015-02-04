/*
  -------------------------------------------------------------------
  
  This file is part of O2scl. It has been adapted from RNS v1.1d
  written by N. Stergioulas and S. Morsink. The modifications made in
  this version from the original are copyright (C) 2014, Andrew W.
  Steiner.
  
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
/*
  -------------------------------------------------------------------
  Relativistic models of rapidly rotating compact stars,
  using tabulated or polytropic equations of state.
  
  Author:  Nikolaos Stergioulas
  
  Current Address:
  
  Department of Physics
  University of Wisconsin-Milwaukee
  PO Box 413, Milwaukee, WI 53201, USA
  
  E-mail: niksterg@csd.uwm.edu, or
  niksterg@pauli.phys.uwm.edu
  
  Version: 1.1
  
  Date:    June, 1995
  
  Changes made to code by Sharon Morsink
   
  03-03-97: Corrected the units for polytropic stars
  10-28-98: Added the star's quadrupole moment to the output.
  
  References:
  KEH : H. Komatsu, Y. Eriguchi and I. Hachisu, Mon. Not. R. astr. Soc. 
  (1989) 237, 355-379.
  CST : G. Cook, S. Shapiro and S. Teukolsky, Ap. J (1992) 398, 203-223.
  
  -------------------------------------------------------------------
*/
#ifndef RNS_H
#define RNS_H

#include <cmath>
#include <iostream>

#include <o2scl/search_vec.h>
#include <o2scl/test_mgr.h>
#include <o2scl/root_bkt_cern.h>

namespace o2scl {

  /** \brief Rotating neutron star class based on RNS v1.1d from
      N. Stergioulas et al.

      \note This class is still experimental.

      Several changes have been made to the original code. The code
      using Numerical Recipes has been removed and replaced with an
      equivalent based on GSL and O2scl. The overall interface has
      been changed, some code has been updated with C++ equivalents,
      constants have been updated with more recent values, and the
      coding style has been changed.

      <b>Quadrupole moments</b>

      Quadrupole moments computed using the method in \ref Laarakkers99. 

      <b>Axisymmetric Instability</b>

      \ref Friedman88 shows that a secular axisymmetric instability
      sets in when the mass becomes maximum along a sequence of
      constant angular momentum. Equivalently, \ref Cook92 shows that
      the instability occurs when the angular momentum becomes minimum
      along a sequence of constant rest mass.

      A GR virial theorem for a stationary and axisymmetric system was
      found in \ref Bonazzola73. A more general two-dimensional virial
      identity was found in \ref Bonazzola94. The three-dimensional
      virial identity found in \ref Gourgoulhon94 is a generalization
      of the Newtonial virial theorem.

      Using the stationary and axisymmetric metric ( \f$ G = c = 1 \f$
      )
      \f[
      ds^2 = - e^{\gamma+\rho} dt^2 + e^{2 \alpha} \left( dr^2 + 
      r^2 d\theta^2 \right) + e^{\gamma-\rho} r^2 \sin^2 \theta
      ( d \phi - \omega dt) ^2
      \f]
      one solves for the four metric functions \f$ \rho(r,\theta) \f$,
      \f$ \gamma(r,\theta) \f$, \f$ \alpha(r,\theta) \f$ and \f$
      \omega(r,\theta) \f$ .

      It is assumed that matter is a perfect fluid, and the 
      stress-energy tensor is
      \f[
      T^{\mu \nu} = \left( \rho_0 + \rho_i + P \right) u^{\mu} u^{\nu}
      + P g^{\mu \nu}
      \f]
      
      Einstein's field equations imply four field equations for
      a specified rotation law,
      \f[
      u^{t} u_{\phi} = F(\Omega) 
      \f]
      for some function \f$ F(\omega) \f$ .

      Using Eq. (27) in \ref Cook92, one can write
      \f[
      \rho(s,\mu) = - e^{-\gamma/2} \sum_{n=0}^{\infty}
      P_{2n}(\mu) \int_0^{1}~ds^{\prime} \int_0^1~d \mu 
      f_{\rho}(n,s,s^{\prime}) P_{2n}{\mu^{\prime}} 
      \tilde{S}(s^{\prime},\mu^{\prime})
      \f]
      where the function \f$ f_{\rho} \f$ is defined by
      \f[
      f_{\rho} \equiv \Theta(s^{\prime}-s)
      \left(\frac{1-s}{s}\right)^{2 n+1} \left[\frac{s^{\prime
      2n}}{(1-s^{\prime})^{2n+2}}\right] + \Theta(s^{\prime}-s)
      \left(\frac{1-s}{s}\right)^{2 n+1} \left[\frac{s^{\prime
      2n}}{(1-s^{\prime})^{2n+2}}\right]
      \f]
      This function is stored in \ref f_rho . Similar 
      definitions are made for \ref f_gamma and \ref f_omega .

      The Keplerial orbit at the equator is 
      \f[
      \Omega_K = \frac{\omega^{\prime}}{2 \psi^{\prime}} ...
      \f]
      (eq. 31 in \ref Stergioulas03 )

      \future Fix unit-indexed arrays
      \future Try moving some of the storage to the heap
      \future Allow more than 200 points in the tabulated EOS
      \future Some of the arrays seem larger than necessary
      \future The function \ref o2scl::nstar_rot::new_search() is inefficient
      because it has to handle the boundary conditions separately.
      This could be improved.
      \future Give the user more control over the initial guess

      <b>Initial guess</b>

      The original RNS code suggests that the initial guess is
      typically a star with a smaller angular momentum.

      <b>References</b>

  */
  class nstar_rot {
    
  public:    
    
    /// The number of grid points in the \f$ \mu \f$ direction
    static const int MDIV=65;
    /// The number of grid points in the \f$ s \f$ direction
    static const int SDIV=129;
    /// The number of Legendre polynomials
    static const int LMAX=10;

  protected:

    /** \brief Class which specifies the function to invert 
	a polytropic EOS
     */
    class polytrope_solve {

    protected:

      /** \brief The polytropic index
       */
      double _Gamma_P;

      /** \brief Desc
       */
      double _ee;

    public:

      /** \brief Create a function object with specified 
	  polytropic index and ?
      */
      polytrope_solve(double Gamma_P, double ee) {
	_Gamma_P=Gamma_P;
	_ee=ee;
      }
      
      /** \brief The function
       */
      double operator()(double rho0) {
	return pow(rho0,_Gamma_P)/(_Gamma_P-1.0)+rho0-_ee;
      }
      
    };

    /// The polytrope solver
    o2scl::root_bkt_cern<polytrope_solve> rbc;

    /// Search in array \c x of length \c n for value \c val
    int new_search(int n, double *x, double val);
    
    /// Array search object
    o2scl::search_vec<double *> sv;

    /** \brief grid point in RK integration */ 
    static const int RDIV=900;                     
    
    /** \brief Maximum value of s-coordinate (default 0.9999) */  
    double SMAX;
    /** \brief Spacing in \f$ s \f$ direction, 
	\f$ \mathrm{SMAX}/(\mathrm{SDIV}-1) \f$ 
    */
    double DS;
    /** \brief Spacing in \f$ \mu \f$ direction, \f$ 1/(\mathrm{MDIV}-1) \f$ 
     */ 
    double DM;

    /// The constant \f$ \pi \f$
    double PI;

    /// Desc (default \f$ 10^{-15} \f$)
    double RMIN;

    /** \brief Nearest grid point, used in interpolation (default 1) */ 
    int n_nearest;                     
    /** \brief Indicates if iteration diverged (default 0) */ 
    int a_check;                       
    /** \brief 0 if not print dif (default 1) */  
    int print_dif;                       
    /** \brief select print out (default 1) */ 
    int print_option;                    

    /// \name Grid quantities set in make_grid()
    //@{
    /// \f$ s \f$
    double s_gp[SDIV+1];                 
    /// \f$ s (1-s) \f$
    double s_1_s[SDIV+1];
    /// \f$ 1-s \f$
    double one_s[SDIV+1];
    /// \f$ \mu \f$
    double mu[MDIV+1];                   
    /// \f$ 1-\mu^2 \f$
    double one_m2[MDIV+1];
    /// \f$ \theta \f$ defined by \f$ \mathrm{acos}~\mu \f$
    double theta[MDIV+1];
    /// \f$ \sin \theta \f$
    double sin_theta[MDIV+1];
    //@}

    /// \name Metric functions
    //@{
    /** \brief potential \f$ \rho \f$ */ 
    double rho[SDIV+1][MDIV+1];          
    /** \brief potential \f$ \gamma \f$ */ 
    double gamma[SDIV+1][MDIV+1];         
    /** \brief potential \f$ \omega \f$ */ 
    double omega[SDIV+1][MDIV+1];        
    /** \brief potential \f$ \alpha \f$ */ 
    double alpha[SDIV+1][MDIV+1];        
    //@}

    /// \name Initial guess
    //@{
    /// Guess for the equatorial radius
    double r_e_guess;
    /** \brief Guess for \f$ \rho \f$ */ 
    double rho_guess[SDIV+1][MDIV+1];    
    /** \brief Guess for \f$ \gamma \f$ */
    double gamma_guess[SDIV+1][MDIV+1];   
    /** \brief Guess for \f$ \alpha \f$ */
    double omega_guess[SDIV+1][MDIV+1];  
    /** \brief Guess for \f$ \omega \f$ */
    double alpha_guess[SDIV+1][MDIV+1];  
    //@}

    /// \name EOS quantities
    //@{
    /** \brief Energy density \f$ \epsilon \f$ */
    double energy[SDIV+1][MDIV+1];       
    /** \brief Pressure */ 
    double pressure[SDIV+1][MDIV+1];     
    /** \brief Enthalpy */
    double enthalpy[SDIV+1][MDIV+1];     
    //@}

    /// \name Other quantities defined over the full two-dimensional grid
    //@{
    /** \brief Proper velocity squared */
    double velocity_sq[SDIV+1][MDIV+1];  
    /** \brief Derivative of \f$ \alpha \f$ with respect to \f$ \mu \f$ */
    double da_dm[SDIV+1][MDIV+1];
    //@}

    /// \name Quantities defined for fixed values of mu
    //@{
    /** \brief \f$ \gamma(s) \f$ at \f$ \mu=1 \f$ */
    double gamma_mu_1[SDIV+1];            
    /** \brief \f$ \gamma(s) \f$ at \f$ \mu=0 \f$ */
    double gamma_mu_0[SDIV+1];            
    /** \brief \f$ \rho(s) \f$ at \f$ \mu=1 \f$ */
    double rho_mu_1[SDIV+1];             
    /** \brief \f$ \rho(s) \f$ at \f$ \mu=0 \f$ */
    double rho_mu_0[SDIV+1];             
    /** \brief \f$ \omega(s) \f$ at \f$ \mu=0 \f$ */
    double omega_mu_0[SDIV+1];           
    //@}

    /** \brief Radius at pole */      
    double r_p;                          
    /** \brief s-coordinate at pole */
    double s_p;                          
    /** \brief s-coordinate at equator */
    double s_e; 
    /** \brief gamma^hat at pole */  
    double gamma_pole_h;                  
    /** \brief gamma^hat at center */
    double gamma_center_h;                
    /** \brief gamma^hat at equator */
    double gamma_equator_h;               
    /** \brief rho^hat at pole */ 
    double rho_pole_h;                   
    /** \brief rho^hat at center */
    double rho_center_h;                 
    /** \brief rho^hat at equator */ 
    double rho_equator_h;                
    /** \brief omega^hat at equator */
    double omega_equator_h;              
    /** \brief angular velocity \f$ \Omega^hat \f$ */
    double Omega_h;                      
    /** \brief central pressure */ 
    double p_center;                     
    /** \brief central enthalpy */
    double h_center;                     

    /// \name Desc
    //@{
    /** \brief f_rho(s,n,s') */
    double f_rho[SDIV+1][LMAX+1][SDIV+1];
    /** \brief f_gamma(s,n,s') */
    double f_gamma[SDIV+1][LMAX+1][SDIV+1];
    /** \brief f_omega(s,n,s') */
    double f_omega[SDIV+1][LMAX+1][SDIV+1];
    /** \brief Legendre polynomial \f$ P_{2n}(\mu) \f$ 
     */  
    double P_2n[MDIV+1][LMAX+1];         
    /** \brief Associated Legendre polynomial \f$ P^1_{2n-1}(\mu) \f$ 
     */ 
    double P1_2n_1[MDIV+1][LMAX+1];      
    //@}

    /// Desc
    double velocity_equator;              
    /// Proper mass
    double Mass_p;
    /** \brief used in guess */
    double r_final;
    /// Desc
    double m_final;
    /// Desc
    double r_is_final;
    /// Desc
    double r_gp[RDIV+1];
    /// Desc
    double r_is_gp[RDIV+1];
    /// Desc
    double m_gp[RDIV+1];
    /// Desc
    double lambda_gp[RDIV+1];
    /// Desc
    double e_d_gp[RDIV+1];   
    /// Desc
    double nu_gp[RDIV+1];
    /// Desc
    double gamma_s[SDIV+1];
    /// Desc
    double rho_s[SDIV+1];
    /// Desc
    double da_dm_s[MDIV+1];         
    /** \brief temporary storage of derivative */
    double d_temp;                      
    /** \brief accuracy in r_e (default \f$ 10^{-5} \f$) */
    double accuracy;                    

    /** \brief integrated term over m in eqn for rho */
    double D1_rho[LMAX+1][SDIV+1];  
    /** \brief integrated term over m in eqn for gamma */
    double D1_gamma[LMAX+1][SDIV+1]; 
    /** \brief integ. term over m in eqn for omega */
    double D1_omega[LMAX+1][SDIV+1];
    /** \brief integrated term over s in eqn for rho */
    double D2_rho[SDIV+1][LMAX+1];  
    /** \brief integrated term over s in eqn for gamma */
    double D2_gamma[SDIV+1][LMAX+1]; 
    /** \brief integ. term over s in eqn for omega */
    double D2_omega[SDIV+1][LMAX+1];

    /** \brief source term in eqn for gamma */
    double S_gamma[SDIV+1][MDIV+1];  
    /** \brief source term in eqn for rho */
    double S_rho[SDIV+1][MDIV+1];   
    /** \brief source term in eqn for omega */
    double S_omega[SDIV+1][MDIV+1]; 

    /// Desc
    double v_plus[SDIV+1];
    /// Desc
    double v_minus[SDIV+1];

    /// Desc
    double vel_plus;
    /// Desc
    double vel_minus;
    /// Desc
    double sign;
    /// Desc
    double dr;
    /// Desc
    double omega_error;
    /// Desc
    double h_error;
    /// Desc
    double M_0const;
    /// Desc
    double J_const;
    /// Desc
    double M_0_error;
    /// Desc
    double M_error;
    /// Desc
    double J_error;
    /// Desc
    double dgds[SDIV+1][MDIV+1];
    /// Desc
    double dgdm[SDIV+1][MDIV+1];
    /// Desc
    double Omega_const;
    /// Desc (default \f$ 10^{-4} \f$ )
    double fix_error;

    /// \name Set in the run() function
    //@{
    /// Desc
    double p_surface;
    /// Desc
    double e_surface;
    /** \brief min. enthalpy in h file */
    double enthalpy_min;                 
    //@}
    /// Polytropic index
    double n_P;
    /// Polytropic exponent
    double Gamma_P;
    /// Desc
    double rho0_center;
    /// Desc
    double eccentricity;
    /// Desc
    double grv2;
    /// Desc
    double grv2_new;
    /// Desc
    double grv3;

    /// \name These are only used when CL_LOW is true   
    //@{
    /// Desc
    double e_match;
    /// Desc
    double p_match;
    /// Desc
    double h_match;
    /// Desc
    double n0_match;
    //@}

    /// Desc
    double de_pt;
    /// Desc
    double e_cl;

    /** \brief Create computational mesh. 

	Create the computational mesh for \f$ s=r/(r+r_e) \f$
	(where \f$ r_e \f$ is the coordinate equatorial radius) 
	and \f$ \mu = \cos \theta \f$
	using 
	\f[
	s[i]=\mathrm{SMAX}\left(\frac{i-1}{\mathrm{SDIV}-1}\right)
	\f]
	\f[
	\mu[j]=\left(\frac{i-1}{\mathrm{MDIV}-1}\right)
	\f]
	When \f$ r=0 \f$, \f$ s=0 \f$, when \f$ r=r_e \f$, 
	\f$ s=1/2 \f$, and when \f$ r = \infty \f$, \f$ s=1 \f$ .
	(Note that some versions of the manual have a typo,
	giving \f$ 1-i \f$ rather than \f$ i-1 \f$ above.)
	
	Points in the mu-direction are stored in the array
	<tt>mu[i]</tt>. Points in the s-direction are stored in the
	array <tt>s_gp[j]</tt>.

	This function sets \ref s_gp, \ref s_1_s, \ref one_s,
	\ref mu, \ref one_m2, \ref theta and \ref sin_theta .
	All of these arrays are unit-indexed.
    */
    void make_grid();

    /** \brief Desc */
    double e_of_rho0(double rho0);
 
    /** \brief Driver for the interpolation routine. 
	
	First we find the tab. point nearest to xb, then we
	interpolate using four points around xb.
	
	Used by \ref int_z(), \ref e_at_p(), \ref p_at_e(), 
	\ref p_at_h(), \ref h_at_p(), \ref n0_at_e(), 
	\ref comp_omega(), \ref comp_M_J(), \ref comp(), 
	\ref guess(), \ref iterate(), and \ref run().
    */  
    double interp(double xp[], double yp[], int np ,double xb);

    /** \brief Driver for the interpolation routine.

	Four point interpolation at a 
	given offset the index of the first point k. 

	Used in \ref comp() .
    */
    double interp_4_k(double xp[], double yp[], int np, double xb, int k);

    /** \brief Integrate f[mu] from m-1 to m. 

	This implements a 8-point closed Newton-Cotes formula.
	
	Used in \ref comp() .
    */
    double int_z(double f[MDIV+1], int m);

    /** \brief Compute \f$ \varepsilon(P) \f$  
	
	Used in \ref dm_dr_is(), \ref dp_dr_is(), \ref integrate()
	and \ref iterate(). 
    */
    double e_at_p(double pp);

    /** \brief Compute \f$ P(\varepsilon) \f$  
	
	Used in \ref make_center() and \ref integrate().
    */
    double p_at_e(double ee);

    /** \brief Pressure at fixed enthalpy

	Used in \ref iterate().
     */
    double p_at_h(double hh);

    /** \brief Enthalpy at fixed pressure 

	Used in \ref make_center() and \ref integrate().
    */
    double h_at_p(double pp);
    
    /** \brief Baryon density at fixed energy density 

	Used in \ref comp_M_J() and \ref comp() .
     */
    double n0_at_e(double ee);

    /** \brief Returns the derivative w.r.t. s of an array f[SDIV+1]. 
     */ 
    double s_deriv(double f[SDIV+1], int s);

    /** \brief Returns the derivative w.r.t. mu of an array f[MDIV+1]. 
     */ 
    double m_deriv(double f[MDIV+1], int m);

    /** \brief Returns the derivative w.r.t. s  
     */ 
    double deriv_s(double f[SDIV+1][MDIV+1], int s, int m);

    /** \brief Returns the derivative w.r.t. mu 
     */ 
    double deriv_m(double f[SDIV+1][MDIV+1], int s, int m);

    /** \brief Returns the derivative w.r.t. s and mu 
     */ 
    double deriv_sm(double f[SDIV+1][MDIV+1], int s, int m);

    /** \brief Returns the Legendre polynomial of degree n, evaluated at x. 

	This uses the recurrence relation.
    */
    double legendre(int n, double x);

    /** \brief Compute two-point functions
	
	This function computes the 2-point functions \f$
	f^m_{2n}(r,r') \f$ used to integrate the potentials \f$ \rho,
	\gamma \f$ and \f$ \omega \f$ (See KEH for details). Since the
	grid points are fixed, we can compute the functions \ref
	f_rho, \ref f_gamma, \ref f_omega, \ref P_2n, and \ref P1_2n_1
	once at the beginning.

	See eqs (27)-(29) of CST and (33) - (35) of KEH 
    */
    void comp_f_P(void);

    /** \brief Compute central pressure and enthalpy from central
	energy density

	For polytropic EOSs, this also computes \ref rho0_center .
    */
    void make_center(double e_center);

    /** \brief Compute Omega and Omega_K. 
     */
    void comp_omega(void);
  
    /** \brief Compute rest mass and angular momentum. 
     */
    void comp_M_J(void);

    /** \brief Compute various quantities.

	The main post-processing funciton
     */
    void comp(void);

    /** \brief Desc */
    double dm_dr_is(double r_is, double r, double m, double p);
 
    /** \brief Desc */
    double dp_dr_is(double r_is, double r, double m, double p);

    /** \brief Desc */
    double dr_dr_is(double r_is, double r, double m);

    /** \brief Desc */
    void integrate(int i_check);

    /** \brief Computes a spherically symmetric star 
	
	The metric is 
	\f[
	ds^2 = -e^{2\nu}dt^2 + e^{2\lambda} dr^2 + r^2 d\theta^2 + 
	r^2 sin^2\theta d\phi^2
	\f]
	where \f$ r \f$ is an isotropic radial coordinate 
	(corresponding to <tt>r_is</tt> in the code).
    */
    void guess(void);

    /** \brief Main iteration cycle. 
     */
    int iterate(double r_ratio);

    /// \name Solvers 
    //@{
    /** \brief Compute m/s model for current e_center. 
     */
    void ms_model(void);

    /** \brief Compute h+ = 0 model for current e_center. 
     */
    void h_model(void);

    /** \brief Compute intermediate model for given Mass_0 and e_center. 
     */
    void m0_model(double M_0);

    /** \brief Compute intermediate model for given Mass and e_center. 
     */
    int m_model(double M_fix);

    /** \brief Compute intermediate model for given Omega and e_center. 
     */
    void omega_model(double Omega_const);

    /** \brief Compute model for given J and e_center. 
     */
    int J_model(double J_const);
    //@}

  public:
    
    nstar_rot();

    /// \name Settings
    //@{
    /** \brief Desc (default false)
     */
    bool CL_LOW;
    /// The convergence factor (default 1.0)
    double cf;
    //@}

    /// \name Tabulated EOS
    //@{
    /** \brief If true, use a tabulated EOS (default true)
     */
    bool tabulated_eos;
    /** \brief number of tabulated EOS points */
    int n_tab;                           
    /** \brief rho points in tabulated EOS */
    double log_e_tab[201];               
    /** \brief p points in tabulated EOS */
    double log_p_tab[201];               
    /** \brief h points in EOS file */
    double log_h_tab[201];               
    /** \brief number density in EOS file */  
    double log_n0_tab[201];              
    //@}

    /// \name Hard-coded EOSs for testing
    //@{
    /// Desc
    void eosC();
    /// Desc
    void eosL();
    //@}

    /// \name Output
    //@{
    /** \brief Kepler rotation frequency (in 1/s) */  
    double Omega_K;                      
    /** \brief Central energy density (in \f$ \mathrm{g}/\mathrm{cm}^3 \f$) 
     */
    double e_center;                     
    /// Gravitational mass (in g)
    double Mass;
    /// Baryonic mass (in g)
    double Mass_0;
    /** \brief Ratio of polar to equatorial radius
     */ 
    double r_ratio;                      
    /// Angular momentum
    double J;
    /// Total rotational kinetic energy
    double T;
    /// Moment of inertia
    double I;
    /// Gravitational binding energy
    double W;
    /// Polar redshift
    double Z_p;
    /// Forward equatorial redshift
    double Z_f;
    /// Backward equatorial redshift
    double Z_b;
    /// Angular velocity
    double Omega;
    /** \brief Height from surface of last stable co-rotating circular 
	orbit in equatorial plane

	If this is zero then all orbits are stable.
    */
    double h_plus;
    /** \brief Height from surface of last stable counter-rotating circular 
	orbit in equatorial plane
	
	If this is zero then all orbits are stable.
    */
    double h_minus;
    /// Desc
    double mass_quadrupole;
    /** \brief Radius at equator 
     */ 
    double r_e;                          
    /** \brief Circumferential radius (i.e. the radius defined such
	that \f$ 2 \pi R_e \f$ is the proper circumference) */
    double R_e;                          
    /// Desc
    double om_over_Om;
    /// Desc
    double Omega_plus;
    /// Angular velocity of a particle in a circular orbit at the equator
    double Omega_p;
    /// Desc
    double u_phi;
    /// Desc
    double schwarz;
    //@}

    /// \name Constants
    //@{
    /** \brief Use the values of the constants from the original RNS
	code
    */
    void original_constants();
    /** Speed of light in vacuum (in CGS units) */ 
    double C;
    /** Gravitational constant (in CGS units) */ 
    double G;
    /** Mass of sun (in g) */
    double MSUN;
    /** \brief Square of length scale in CGS units, 
	\f$ \kappa \equiv 10^{-15} c^2/G \f$
    */
    double KAPPA;
    /// The mass of one baryon (in g)
    double MB;
    /** \brief The value \f$ \kappa G c^{-4} \f$ */
    double KSCALE;
    //@}

    /** \brief Function representing old main() function
     */
    int run(int argc, char const **argv);

    /// \name Testing functions
    //@{
    /** \brief Test determining configuration with fixed central
	energy density and fixed radius ratio with EOS C
	
	Compares with
	<tt>rns -f eosC -t model -e 2e15 -r 0.59 -d 0 </tt>
    */    
    void test1(o2scl::test_mgr &t);
    
    /** \brief Test configuration rotating and Keplerian frequency
	with a fixed central energy density and EOS C

	Compares with
	<tt>rns -f eosC -t kepler -e 2e15 -d 0</tt>
     */    
    void test2(o2scl::test_mgr &t);
    
    /** \brief Test fixed central energy density and fixed 
	gravitational mass with EOS C

	Compares with
	<tt>rns -f eosC -t gmass -e 1e15 -m 1.5 -d 0  </tt>
     */    
    void test3(o2scl::test_mgr &t);
    
    /** \brief Test fixed central energy density and fixed baryonic 
	mass with EOS C
	
	Compares with
	<tt>rns -f eosC -t rmass -e 1e15 -z 1.55 -d 0</tt>
     */    
    void test4(o2scl::test_mgr &t);
    
    /** \brief Test fixed central energy density and fixed angular
	velocity with EOS C

	Compares with
	<tt>rns -f eosC -t omega -e 1e15 -o 0.5 -d 0 </tt>
     */    
    void test5(o2scl::test_mgr &t);
    
    /** \brief Test fixed central energy density and fixed angular 
	momentum with EOS C

	Compares with
	<tt>rns -f eosC -t jmoment -e 1e15 -j 1.5 -d 0 </tt>
     */    
    void test6(o2scl::test_mgr &t);

    /** \brief Test a series of non-rotating stars on a energy density
	grid with EOS C

	Compares with
	<tt>rns -f eosC -t static -e 6e14 -l 2e15 -n 2 -p 2 -d 0</tt>
     */    
    void test7(o2scl::test_mgr &t);

    /** \brief Test Keplerian frequency for a polytrope

	Compares with
	<tt>rns -q poly -N 1.0 -e 0.137 -t kepler -d 0</tt>
     */    
    void test8(o2scl::test_mgr &t);
    //@}

  };

}

#endif
