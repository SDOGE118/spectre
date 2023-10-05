// Distributed under the MIT License.
// See LICENSE.txt for details.

#include "Executables/Examples/RandomAmr/RandomAmr.hpp"

#include <vector>

#include "Domain/Creators/RegisterDerivedWithCharm.hpp"
#include "Parallel/CharmMain.tpp"
#include "ParallelAlgorithms/Amr/Actions/RegisterCallbacks.hpp"
#include "Utilities/Serialization/RegisterDerivedClassesWithCharm.hpp"

// Parameters chosen in CMakeLists.txt
using metavariables = RandomAmrMetavars<DIM>;

extern "C" void CkRegisterMainModule() {
  Parallel::charmxx::register_main_module<metavariables>();
  Parallel::charmxx::register_init_node_and_proc(
      {&domain::creators::register_derived_with_charm,
       &amr::register_callbacks<metavariables,
                                typename metavariables::dg_element_array>,
       &register_factory_classes_with_charm<metavariables>},
      {});
}
