#include "third_party/optim/header_only_version/optim.hpp"

 
double myEvaluation(const arma::vec& vals_inp, arma::vec* grad_out, void* opt_data)
{
    const double x = vals_inp(0);
    const double y = vals_inp(1);
 
    double obj_val = 100*std::sqrt(std::abs(y - 0.01*x*x)) + 0.01*std::abs(x + 10);
    //
    return obj_val;
}
 
int main()
{
  optim::algo_settings_t hi;
  printf("%lu\n", sizeof(hi));
  /*
  optim::algo_settings_t settings_1;

  settings_1.pso_center_particle = false;
  settings_1.pso_par_bounds = true;

  arma::vec x_1 = arma::zeros(2,1);

  settings_1.pso_lb = arma::zeros(2,1) - 10.0;
  settings_1.pso_ub = arma::zeros(2,1) + 10.0;

  settings_1.pso_n_pop = 5000;
  settings_1.pso_n_gen = 4000;

  bool success_1 = optim::pso(x_1,myEvaluation,nullptr,settings_1);
  */
  return 0;
}