//=======================================================================
// Copyright (c) 2014 Baptiste Wicht
// Distributed under the terms of the MIT License.
// (See accompanying file LICENSE or copy at
//  http://opensource.org/licenses/MIT)
//=======================================================================

#ifndef DBN_LAYER_HPP
#define DBN_LAYER_HPP

#include "base_conf.hpp"
#include "contrastive_divergence.hpp"
#include "watcher.hpp"
#include "tmp.hpp"

namespace dll {

/*!
 * \brief Describe a RBM layer
 *
 * This struct should be used to define a RBM either as standalone or for a DBN.
 * Once configured, the ::rbm_t member returns the type of the configured RBM.
 */
template<std::size_t visibles, std::size_t hiddens, typename... Parameters>
struct layer {
    static constexpr const std::size_t num_visible = visibles;
    static constexpr const std::size_t num_hidden = hiddens;

    static constexpr const bool Momentum = detail::is_present<momentum, Parameters...>::value;
    static constexpr const std::size_t BatchSize = detail::get_value<batch_size<1>, Parameters...>::value;
    static constexpr const unit_type visible_unit = detail::get_value<visible<unit_type::BINARY>, Parameters...>::value;
    static constexpr const unit_type hidden_unit = detail::get_value<hidden<unit_type::BINARY>, Parameters...>::value;
    static constexpr const decay_type Decay = detail::get_value<weight_decay<decay_type::NONE>, Parameters...>::value;
    static constexpr const bool Init = detail::is_present<init_weights, Parameters...>::value;
    static constexpr const bool DBN = detail::is_present<in_dbn, Parameters...>::value;
    static constexpr const bool Sparsity = detail::is_present<sparsity, Parameters...>::value;

    /*! The type of the trainer to use to train the RBM */
    template <typename RBM>
    using trainer_t = typename detail::get_template_type<trainer<cd1_trainer_t>, Parameters...>::template type<RBM>;

    /*! The type of the watched to use during training */
    template <typename RBM>
    using watcher_t = typename detail::get_template_type<watcher<default_rbm_watcher>, Parameters...>::template type<RBM>;

    /*! The RBM type */
    using rbm_t = rbm<layer<visibles, hiddens, Parameters...>>;

    static_assert(num_visible > 0, "There must be at least 1 visible unit");
    static_assert(num_hidden > 0, "There must be at least 1 hidden unit");

    //Make sure only valid types are passed to the configuration list
    static_assert(
        detail::is_valid<detail::tmp_list<momentum, batch_size_id, visible_id, hidden_id, weight_decay_id,
              init_weights, in_dbn, sparsity, trainer_id>, Parameters...>::value,
        "Invalid parameters type");

    static_assert(BatchSize > 0, "Batch size must be at least 1");

    static_assert(!Sparsity || (Sparsity && hidden_unit == unit_type::BINARY),
        "Sparsity only works with binary hidden units");
};

} //end of dbn namespace

#endif