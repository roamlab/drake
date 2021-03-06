#pragma once

#include <memory>
#include <stdexcept>
#include <string>

#include "drake/common/drake_copyable.h"
#include "drake/common/drake_deprecated.h"
#include "drake/multibody/multibody_tree/multibody_plant/multibody_plant.h"
#include "drake/systems/controllers/inverse_dynamics.h"
#include "drake/systems/controllers/pid_controller.h"
#include "drake/systems/controllers/state_feedback_controller_interface.h"
#include "drake/systems/framework/diagram.h"

#ifndef DRAKE_DOXYGEN_CXX
// Forward declaration because we only need the type name for deprecation
// purposes; we never call any methods on an RBT.
template <class T>
class RigidBodyTree;
#endif

namespace drake {
namespace systems {
namespace controllers {

/**
 * A state feedback controller that uses a PidController to generate desired
 * accelerations, which are then converted into torques using InverseDynamics.
 * More specifically, the output of this controller is:
 * `torque = inverse_dynamics(q, v, vd_d)`,
 * where `vd_d = kp(q* - q) + kd(v* - v) + ki int(q* - q) + vd*`.
 * `q` and `v` stand for the generalized position and velocity, and `vd` is
 * the generalized acceleration. `*` indicates reference values.
 *
 * This controller always has a BasicVector input port for estimated robot state
 * `(q, v)`, a BasicVector input port for reference robot state `(q*, v*)` and
 * a BasicVector output port for computed torque `torque`. A constructor flag
 * can be set to track reference acceleration `vd*` as well. When set, a
 * BasicVector input port is also declared, and it's content is used as `vd*`.
 * When unset, `vd*` is be treated as zero.
 *
 * Note that this class assumes the robot is fully actuated, its position
 * and velocity have the same dimension, and it does not have a floating base.
 * If violated, the program will abort. This controller was not designed for
 * closed loop systems: the controller accounts for neither constraint forces
 * nor actuator forces applied at loop constraints. Use on such systems is not
 * recommended.
 *
 * @tparam T The vector element type, which must be a valid Eigen scalar.
 * @see InverseDynamics for an accounting of all forces incorporated into the
 *      inverse dynamics computation.
 *
 * Instantiated templates for the following kinds of T's are provided:
 *
 * - double
 *
 * @ingroup control_systems
 */
template <typename T>
class InverseDynamicsController : public Diagram<T>,
                                  public StateFeedbackControllerInterface<T> {
 public:
  DRAKE_NO_COPY_NO_MOVE_NO_ASSIGN(InverseDynamicsController)

#ifndef DRAKE_DOXYGEN_CXX
  // TODO(jwnimmer-tri) Remove these stubs on or about 2019-03-01.
  // Remember to remove the forward declaration above at the same time.
  DRAKE_DEPRECATED(
      "Inverse dynamics for RigidBodyTree no longer uses this class; for new "
      "instructions, see https://github.com/RobotLocomotion/drake/pull/9987")
  InverseDynamicsController(std::unique_ptr<RigidBodyTree<T>>,
                            const VectorX<double>&, const VectorX<double>&,
                            const VectorX<double>&, bool) {
    throw std::runtime_error(
        "Inverse dynamics for RigidBodyTree no longer uses this class; for new "
        "instructions, see https://github.com/RobotLocomotion/drake/pull/9987");
  }
#endif

  /**
   * Constructs an inverse dynamics controller for the given `plant` model.
   * The %InverseDynamicsController holds an internal, non-owned reference to
   * the MultibodyPlant object so you must ensure that `plant` has a longer
   * lifetime than `this` %InverseDynamicsController.
   * @param plant The model of the plant for control.
   * @param kp Position gain.
   * @param ki Integral gain.
   * @param kd Velocity gain.
   * @param has_reference_acceleration If true, there is an extra BasicVector
   * input port for `vd*`. If false, `vd*` is treated as zero, and no extra
   * input port is declared.
   * @pre `plant` has been finalized (plant.is_finalized() returns `true`).
   * @throws std::exception if
   *  - The plant is not finalized (see MultibodyPlant::Finalize()).
   *  - The number of generalized velocities is not equal to the number of
   *    generalized positions.
   *  - The model is not fully actuated.
   *  - Vector kp, ki and kd do not all have the same size equal to the number
   *    of generalized positions.
   */
  InverseDynamicsController(
      const multibody::multibody_plant::MultibodyPlant<T>& plant,
      const VectorX<double>& kp,
      const VectorX<double>& ki,
      const VectorX<double>& kd,
      bool has_reference_acceleration);

  ~InverseDynamicsController() override;

  /**
   * Sets the integral part of the PidController to @p value.
   * @p value must be a column vector of the appropriate size.
   */
  void set_integral_value(Context<T>* context,
                          const Eigen::Ref<const VectorX<T>>& value) const;

  /**
   * Returns the input port for the reference acceleration.
   */
  const InputPort<T>& get_input_port_desired_acceleration() const {
    DRAKE_DEMAND(has_reference_acceleration_);
    DRAKE_DEMAND(input_port_index_desired_acceleration_ >= 0);
    return Diagram<T>::get_input_port(input_port_index_desired_acceleration_);
  }

  /**
   * Returns the input port for the estimated state.
   */
  const InputPort<T>& get_input_port_estimated_state() const final {
    return this->get_input_port(input_port_index_estimated_state_);
  }

  /**
   * Returns the input port for the desired state.
   */
  const InputPort<T>& get_input_port_desired_state() const final {
    return this->get_input_port(input_port_index_desired_state_);
  }

  /**
   * Returns the output port for computed control.
   */
  const OutputPort<T>& get_output_port_control() const final {
    return this->get_output_port(output_port_index_control_);
  }

#ifndef DRAKE_DOXYGEN_CXX
  DRAKE_DEPRECATED(
      "Inverse dynamics for RigidBodyTree no longer uses this class; for new "
      "instructions, see https://github.com/RobotLocomotion/drake/pull/9987")
  const RigidBodyTree<T>& get_robot_for_control() const {
    throw std::runtime_error(
        "Inverse dynamics for RigidBodyTree no longer uses this class; for new "
        "instructions, see https://github.com/RobotLocomotion/drake/pull/9987");
  }

  DRAKE_DEPRECATED(
      "Inverse dynamics for RigidBodyTree no longer uses this class; for new "
      "instructions, see https://github.com/RobotLocomotion/drake/pull/9987")
  const RigidBodyTree<T>* get_rigid_body_tree_for_control() {
    throw std::runtime_error(
        "Inverse dynamics for RigidBodyTree no longer uses this class; for new "
        "instructions, see https://github.com/RobotLocomotion/drake/pull/9987");
  }
#endif

  /**
   * Returns a constant pointer to the MultibodyPlant used for control.
   */
  const multibody::multibody_plant::MultibodyPlant<T>*
      get_multibody_plant_for_control() const {
    return multibody_plant_for_control_;
  }

 private:
  void SetUp(const VectorX<double>& kp, const VectorX<double>& ki,
      const VectorX<double>& kd,
      const controllers::InverseDynamics<T>& inverse_dynamics,
      DiagramBuilder<T>* diagram_builder);

  const multibody::multibody_plant::MultibodyPlant<T>*
      multibody_plant_for_control_{nullptr};
  PidController<T>* pid_{nullptr};
  const bool has_reference_acceleration_{false};
  int input_port_index_estimated_state_{-1};
  int input_port_index_desired_state_{-1};
  int input_port_index_desired_acceleration_{-1};
  int output_port_index_control_{-1};
};

}  // namespace controllers
}  // namespace systems
}  // namespace drake
