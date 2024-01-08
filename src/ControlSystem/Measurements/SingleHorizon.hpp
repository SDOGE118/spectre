// Distributed under the MIT License.
// See LICENSE.txt for details.

#pragma once

#include <cstddef>
#include <string>

#include "ControlSystem/Protocols/Measurement.hpp"
#include "ControlSystem/Protocols/Submeasurement.hpp"
#include "ControlSystem/RunCallbacks.hpp"
#include "Domain/Structure/ObjectLabel.hpp"
#include "ParallelAlgorithms/ApparentHorizonFinder/Callbacks/ErrorOnFailedApparentHorizon.hpp"
#include "ParallelAlgorithms/ApparentHorizonFinder/Callbacks/FindApparentHorizon.hpp"
#include "ParallelAlgorithms/ApparentHorizonFinder/ComputeHorizonVolumeQuantities.hpp"
#include "ParallelAlgorithms/ApparentHorizonFinder/HorizonAliases.hpp"
#include "ParallelAlgorithms/ApparentHorizonFinder/InterpolationTarget.hpp"
#include "ParallelAlgorithms/Interpolation/Events/Interpolate.hpp"
#include "ParallelAlgorithms/Interpolation/Protocols/InterpolationTargetTag.hpp"
#include "Time/Tags/TimeAndPrevious.hpp"
#include "Utilities/ProtocolHelpers.hpp"
#include "Utilities/TMPL.hpp"

/// \cond
class DataVector;
template <size_t VolumeDim>
class ElementId;
template <size_t Dim>
class Mesh;
namespace Parallel {
template <typename Metavariables>
class GlobalCache;
}  // namespace Parallel
namespace domain::Tags {
template <size_t Dim>
struct Mesh;
}  // namespace domain::Tags
/// \endcond

namespace control_system::measurements {
/*!
 * \brief A `control_system::protocols::Measurement` that relies on only one
 * apparent horizon; the template parameter `Horizon`.
 */
template <::domain::ObjectLabel Horizon>
struct SingleHorizon : tt::ConformsTo<protocols::Measurement> {
  static std::string name() {
    return "SingleHorizon" + ::domain::name(Horizon);
  }

  /*!
   * \brief A `control_system::protocols::Submeasurement` that starts the
   * interpolation to the interpolation target in order to find the apparent
   * horizon.
   */
  struct Submeasurement : tt::ConformsTo<protocols::Submeasurement> {
    static std::string name() { return SingleHorizon::name(); }

   private:
    template <typename ControlSystems>
    struct InterpolationTarget
        : tt::ConformsTo<intrp::protocols::InterpolationTargetTag> {
      static std::string name() {
        return "ControlSystemSingleAh" + ::domain::name(Horizon);
      }

      using temporal_id = ::Tags::TimeAndPrevious<0>;

      using vars_to_interpolate_to_target =
          ::ah::vars_to_interpolate_to_target<3, ::Frame::Distorted>;
      using compute_vars_to_interpolate = ::ah::ComputeHorizonVolumeQuantities;
      using tags_to_observe = ::ah::tags_for_observing<Frame::Distorted>;
      using compute_items_on_target =
          ::ah::compute_items_on_target<3, Frame::Distorted>;
      using compute_items_on_source =
          tmpl::list<::Tags::TimeAndPreviousCompute<0>>;
      using compute_target_points =
          intrp::TargetPoints::ApparentHorizon<InterpolationTarget,
                                               ::Frame::Distorted>;
      using post_interpolation_callbacks =
          tmpl::list<intrp::callbacks::FindApparentHorizon<InterpolationTarget,
                                                           ::Frame::Distorted>>;
      using horizon_find_failure_callback =
          intrp::callbacks::ErrorOnFailedApparentHorizon;
      using post_horizon_find_callbacks = tmpl::list<
          control_system::RunCallbacks<Submeasurement, ControlSystems>>;
    };

   public:
    template <typename ControlSystems>
    using interpolation_target_tag = InterpolationTarget<ControlSystems>;

    template <typename ControlSystems>
    using event =
        intrp::Events::Interpolate<3, InterpolationTarget<ControlSystems>,
                                   ::ah::source_vars<3>>;
  };

  using submeasurements = tmpl::list<Submeasurement>;
};
}  // namespace control_system::measurements
