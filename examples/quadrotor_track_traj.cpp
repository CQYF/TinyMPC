#include <iostream>

#include <tinympc/admm.hpp>
#include "problem_data/quadrotor_20hz_simple.hpp"
#include "trajectory_data/quadrotor_20hz_figure_eight.hpp"

using Eigen::Matrix;

void populate_matrices(Matrix<tinytype,2,2> mat[], int size) {
    for (int i=0; i<size; i++) {
        mat[i](0,0) = i*4+3;
        mat[i](0,1) = i*4+2;
        mat[i](1,0) = i*4+1;
        mat[i](1,1) = i*4;
    }
}

void print_matrices(Matrix<tinytype,2,2> matrices[], int size) {
    for (int i=0; i<size; i++) {
        std::cout << matrices[i] << "\n" << std::endl;
    }
}

int main() {

    // Copy data from problem_data/quadrotor*.hpp
    struct tiny_cache cache;
    cache.Adyn = Eigen::Map<Matrix<tinytype, NSTATES, NSTATES, Eigen::RowMajor>>(Adyn_data);
    cache.Bdyn = Eigen::Map<Matrix<tinytype, NSTATES, NINPUTS, Eigen::RowMajor>>(Bdyn_data);
    cache.rho = rho_value;
    cache.Kinf = Eigen::Map<Matrix<tinytype, NINPUTS, NSTATES, Eigen::RowMajor>>(Kinf_data);
    cache.Pinf = Eigen::Map<Matrix<tinytype, NSTATES, NSTATES, Eigen::RowMajor>>(Kinf_data);
    cache.Quu_inv = Eigen::Map<Matrix<tinytype, NINPUTS, NINPUTS, Eigen::RowMajor>>(Quu_inv_data);
    cache.AmBKt = Eigen::Map<Matrix<tinytype, NSTATES, NSTATES, Eigen::RowMajor>>(AmBKt_data);
    cache.coeff_d2p = Eigen::Map<Matrix<tinytype, NSTATES, NINPUTS, Eigen::RowMajor>>(coeff_d2p_data);

    struct tiny_params params;
    params.Q = Eigen::Map<Matrix<tinytype, NSTATES, NSTATES, Eigen::RowMajor>>(Q_data);
    params.Qf = Eigen::Map<Matrix<tinytype, NSTATES, NSTATES, Eigen::RowMajor>>(Qf_data);
    params.R = Eigen::Map<Matrix<tinytype, NINPUTS, NINPUTS, Eigen::RowMajor>>(R_data);
    params.u_min = {-0.5, -0.5, -0.5, -0.5};
    params.u_max = {0.5, 0.5, 0.5, 0.5};
    for (int i=0; i<NHORIZON; i++) {
        params.x_min[i] = tiny_VectorNc::Constant(-99999); // Currently unused
        params.x_max[i] = tiny_VectorNc::Zero();
        params.A_constraints[i] = tiny_MatrixNcNx::Zero();
    }
    params.Xref = tiny_MatrixNxNh::Zero();
    params.Uref = tiny_MatrixNuNhm1::Zero();
    params.cache = cache;

    struct tiny_problem problem;
    problem.x = tiny_MatrixNxNh::Zero();
    problem.q = tiny_MatrixNxNh::Zero();
    problem.p = tiny_MatrixNxNh::Zero();
    problem.v = tiny_MatrixNxNh::Zero();
    problem.vnew = tiny_MatrixNxNh::Zero();
    problem.g = tiny_MatrixNxNh::Zero();

    problem.u = tiny_MatrixNuNhm1::Zero();
    problem.r = tiny_MatrixNuNhm1::Zero();
    problem.d = tiny_MatrixNuNhm1::Zero();
    problem.z = tiny_MatrixNuNhm1::Zero();
    problem.znew = tiny_MatrixNuNhm1::Zero();
    problem.y = tiny_MatrixNuNhm1::Zero();

    problem.primal_residual_state = 0;
    problem.primal_residual_input = 0;
    problem.dual_residual_state = 0;
    problem.dual_residual_input = 0;
    problem.abs_tol = 0.001;
    problem.status = 0;
    problem.iter = 0;
    problem.max_iter = 100;
    problem.iters_check_rho_update = 10;

    // Copy reference trajectory into Eigen matrix
    Matrix<tinytype, NSTATES, NTOTAL, Eigen::ColMajor> Xref_total = Eigen::Map<Matrix<tinytype, NTOTAL, NSTATES, Eigen::RowMajor>>(Xref_data).transpose();

    // for (int i=0; i<NHORIZON; i++) {
    //     std::cout << params.x_min[i] << "\n" << std::endl;
    //     std::cout << params.x_max[i] << "\n" << std::endl;
    //     std::cout << params.A_constraints[i] << "\n" << std::endl;
    // }
    // std::cout << Xref_total << std::endl;

    // solve_admm(&problem, &params);
    backward_pass_grad(&problem, &params);


    return 0;
}