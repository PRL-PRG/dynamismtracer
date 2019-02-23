#ifndef PROMISEDYNTRACER_TRACER_STATE_H
#define PROMISEDYNTRACER_TRACER_STATE_H

#include "Argument.h"
#include "Call.h"
#include "Environment.h"
#include "ExecutionContextStack.h"
#include "Function.h"
#include "Variable.h"
#include "sexptypes.h"
#include "stdlibs.h"

#include <unordered_map>

class TracerState {
  private:
    const std::string output_dirpath_;
    const std::string trace_filepath_;
    const bool        enable_trace_;
    const bool        truncate_;
    const bool        verbose_;
    const bool        binary_;
    const int         compression_level_;

  public:
    TracerState(const std::string& output_dirpath,
                const std::string& trace_filepath,
                bool               enable_trace,
                bool               truncate,
                bool               verbose,
                bool               binary,
                int                compression_level)
        : output_dirpath_(output_dirpath)
        , trace_filepath_(trace_filepath)
        , enable_trace_(enable_trace)
        , truncate_(truncate)
        , verbose_(verbose)
        , binary_(binary)
        , compression_level_(compression_level)
        , environment_id_(0)
        , variable_id_(0)
        , timestamp_(0)
        , call_id_counter_(0)
        , denoted_value_id_counter_(0)
        , object_count_(OBJECT_TYPE_TABLE_COUNT, 0) {
        object_count_data_table_ =
            create_data_table(output_dirpath_ + "/" + "object_count",
                              {"type", "count"},
                              truncate_,
                              binary_,
                              compression_level_);

        call_summary_data_table_ =
            create_data_table(output_dirpath_ + "/" + "call_summary",
                              {"function_id",
                               "function_type",
                               "formal_parameter_count",
                               "wrapper",
                               "function_name",
                               "generic_method",
                               "dispatcher",
                               "force_order",
                               "missing_arguments",
                               "return_value_type",
                               "call_count"},
                              truncate_,
                              binary_,
                              compression_level_);

        function_definition_data_table_ =
            create_data_table(output_dirpath_ + "/" + "function_definition",
                              {"function_id", "byte_compiled", "definition"},
                              truncate_,
                              binary_,
                              compression_level_);

        argument_data_table_ =
            create_data_table(output_dirpath_ + "/" + "arguments",
                              {"call_id",
                               "function_id",
                               "value_id",
                               "formal_parameter_position",
                               "actual_argument_position",
                               "argument_type",
                               "expression_type",
                               "value_type",
                               "default",
                               "dot_dot_dot",
                               "preforce",
                               "direct_force",
                               "direct_lookup_count",
                               "direct_metaprogram_count",
                               "indirect_force",
                               "indirect_lookup_count",
                               "indirect_metaprogram_count",
                               "S3_dispatch",
                               "S4_dispatch",
                               "forcing_actual_argument_position",
                               "non_local_return"},
                              truncate_,
                              binary_,
                              compression_level_);

        escaped_argument_data_table_ = create_data_table(
            output_dirpath_ + "/" + "escaped_arguments",
            {"call_id",
             "function_id",
             "return_value_type",
             "formal_parameter_count",
             "formal_parameter_position",
             "actual_argument_position",
             "value_id",
             "class",
             "S3_dispatch",
             "S4_dispatch",
             "argument_type",
             "expression_type",
             "value_type",
             "default",
             "non_local_return",
             "escape",
             "call_depth",
             "promise_depth",
             "nested_promise_depth",
             "forcing_actual_argument_position",
             "preforce",
             "before_escape_force_count",
             "before_escape_metaprogram_count",
             "before_escape_value_lookup_count",
             "before_escape_value_assign_count",
             "before_escape_expression_lookup_count",
             "before_escape_expression_assign_count",
             "before_escape_environment_lookup_count",
             "before_escape_environment_assign_count",
             "after_escape_force_count",
             "after_escape_metaprogram_count",
             "after_escape_value_lookup_count",
             "after_escape_value_assign_count",
             "after_escape_expression_lookup_count",
             "after_escape_expression_assign_count",
             "after_escape_environment_lookup_count",
             "after_escape_environment_assign_count",
             "before_escape_direct_self_scope_mutation_count",
             "before_escape_indirect_self_scope_mutation_count",
             "before_escape_direct_lexical_scope_mutation_count",
             "before_escape_indirect_lexical_scope_mutation_count",
             "before_escape_direct_non_lexical_scope_mutation_count",
             "before_escape_indirect_non_lexical_scope_mutation_count",
             "before_escape_direct_self_scope_observation_count",
             "before_escape_indirect_self_scope_observation_count",
             "before_escape_direct_lexical_scope_observation_count",
             "before_escape_indirect_lexical_scope_observation_count",
             "before_escape_direct_non_lexical_scope_observation_count",
             "before_escape_indirect_non_lexical_scope_observation_count",
             "after_escape_direct_self_scope_mutation_count",
             "after_escape_indirect_self_scope_mutation_count",
             "after_escape_direct_lexical_scope_mutation_count",
             "after_escape_indirect_lexical_scope_mutation_count",
             "after_escape_direct_non_lexical_scope_mutation_count",
             "after_escape_indirect_non_lexical_scope_mutation_count",
             "after_escape_direct_self_scope_observation_count",
             "after_escape_indirect_self_scope_observation_count",
             "after_escape_direct_lexical_scope_observation_count",
             "after_escape_indirect_lexical_scope_observation_count",
             "after_escape_direct_non_lexical_scope_observation_count",
             "after_escape_indirect_non_lexical_scope_observation_count",
             "execution_time"},
            truncate_,
            binary_,
            compression_level_);

        promise_data_table_ =
            create_data_table(output_dirpath_ + "/" + "promises",
                              {"value_id",
                               "argument",
                               "expression_type",
                               "value_type",
                               "scope",
                               "S3_dispatch",
                               "S4_dispatch",
                               "preforce",
                               "force_count",
                               "call_depth",
                               "promise_depth",
                               "nested_promise_depth",
                               "metaprogram_count",
                               "value_lookup_count",
                               "value_assign_count",
                               "expression_lookup_count",
                               "expression_assign_count",
                               "environment_lookup_count",
                               "environment_assign_count",
                               "direct_self_scope_mutation_count",
                               "indirect_self_scope_mutation_count",
                               "direct_lexical_scope_mutation_count",
                               "indirect_lexical_scope_mutation_count",
                               "direct_non_lexical_scope_mutation_count",
                               "indirect_non_lexical_scope_mutation_count",
                               "direct_self_scope_observation_count",
                               "indirect_self_scope_observation_count",
                               "direct_lexical_scope_observation_count",
                               "indirect_lexical_scope_observation_count",
                               "direct_non_lexical_scope_observation_count",
                               "indirect_non_lexical_scope_observation_count",
                               "execution_time"},
                              truncate_,
                              binary_,
                              compression_level_);

        promise_lifecycle_data_table_ =
            create_data_table(output_dirpath_ + "/" + "promise_lifecycle",
                              {"action", "count", "promise_count"},
                              truncate_,
                              binary_,
                              compression_level_);
    }

    ~TracerState() {
        delete object_count_data_table_;
        delete call_summary_data_table_;
        delete function_definition_data_table_;
        delete argument_data_table_;
        delete promise_data_table_;
        delete escaped_argument_data_table_;
        delete promise_lifecycle_data_table_;
    }

    const std::string& get_output_dirpath() const {
        return output_dirpath_;
    }

    const std::string& get_trace_filepath() const {
        return trace_filepath_;
    }

    bool is_trace_enabled() const {
        return enable_trace_;
    }

    bool get_truncate() const {
        return truncate_;
    }

    bool is_verbose() const {
        return verbose_;
    }

    bool is_binary() const {
        return binary_;
    }

    int get_compression_level() const {
        return compression_level_;
    }

    void initialize() {
        serialize_configuration_();
    }

    void cleanup(int error) {
        for (auto iter = promises_.begin(); iter != promises_.end(); ++iter) {
            destroy_promise(iter->second);
        }

        promises_.clear();

        for (auto iter = functions_.begin(); iter != functions_.end(); ++iter) {
            destroy_function_(iter->second);
        }

        functions_.clear();

        serialize_object_count_();

        serialize_promise_lifecycle_summary_();

        if (!get_stack().is_empty()) {
            dyntrace_log_error("stack not empty on tracer exit.")
        }

        if (error) {
            std::ofstream error_file(get_output_dirpath() + "/ERROR");
            error_file << "ERROR";
            error_file.close();
        } else {
            std::ofstream noerror_file(get_output_dirpath() + "/NOERROR");
            noerror_file << "NOERROR";
            noerror_file.close();
        }
    }

    void increment_object_count(sexptype_t type) {
        ++object_count_[type];
    }

  private:
    DataTableStream* object_count_data_table_;
    DataTableStream* promise_data_table_;
    DataTableStream* promise_lifecycle_data_table_;

    void serialize_configuration_() {
        std::ofstream fout(get_output_dirpath() + "/CONFIGURATION",
                           std::ios::trunc);

        auto serialize_row = [&fout](const std::string& key,
                                     const std::string& value) {
            fout << key << "=" << value << std::endl;
        };

        for (const std::string& envvar: ENVIRONMENT_VARIABLES) {
            serialize_row(envvar, to_string(getenv(envvar.c_str())));
        }

        serialize_row("GIT_COMMIT_INFO", GIT_COMMIT_INFO);
        serialize_row("truncate", std::to_string(get_truncate()));
        serialize_row("verbose", std::to_string(is_verbose()));
        serialize_row("binary", std::to_string(is_binary()));
        serialize_row("compression_level",
                      std::to_string(get_compression_level()));
    }

    void serialize_object_count_() {
        for (int i = 0; i < object_count_.size(); ++i) {
            if (object_count_[i] != 0) {
                object_count_data_table_->write_row(
                    sexptype_to_string(i),
                    static_cast<double>(object_count_[i]));
            }
        }
    }

    ExecutionContextStack stack_;

  public:
    ExecutionContextStack& get_stack() {
        return stack_;
    }

    Environment& create_environment(const SEXP rho) {
        auto iter = environment_mapping_.find(rho);
        if (iter != environment_mapping_.end()) {
            return iter->second;
        }
        return environment_mapping_
            .insert({rho, Environment(rho, create_next_environment_id_())})
            .first->second;
    }

    void remove_environment(const SEXP rho) {
        environment_mapping_.erase(rho);
    }

    Environment& lookup_environment(const SEXP rho, bool create = true) {
        auto iter = environment_mapping_.find(rho);
        if (iter == environment_mapping_.end()) {
            return create_environment(rho);
        }
        return iter->second;
    }

    Variable& lookup_variable(const SEXP rho,
                              const SEXP symbol,
                              bool       create_environment = true,
                              bool       create_variable    = true) {
        return lookup_variable(
            rho, symbol_to_string(symbol), create_environment, create_variable);
    }

    Variable& lookup_variable(const SEXP         rho,
                              const std::string& symbol,
                              bool               create_environment = true,
                              bool               create_variable    = true) {
        Environment& env = lookup_environment(rho, create_environment);

        bool var_exists = env.exists(symbol);

        if (create_variable && !var_exists) {
            return env.define(
                symbol, create_next_variable_id_(), get_current_timestamp_());
        }

        return env.lookup(symbol);
    }

    Variable& define_variable(const SEXP rho,
                              const SEXP symbol,
                              bool       create_environment = true) {
        return lookup_environment(rho, true).define(symbol_to_string(symbol),
                                                    create_next_variable_id_(),
                                                    get_current_timestamp_());
    }

    Variable& update_variable(const SEXP rho,
                              const SEXP symbol,
                              bool       create_environment = true,
                              bool       create_variable    = true) {
        Variable& var = lookup_variable(rho, symbol);
        var.set_modification_timestamp(get_current_timestamp_());
        return var;
    }

    Variable remove_variable(const SEXP rho,
                             const SEXP symbol,
                             bool       create_environment = true) {
        return lookup_environment(rho, create_environment)
            .remove(symbol_to_string(symbol));
    }

  private:
    env_id_t create_next_environment_id_() {
        return environment_id_++;
    }

    var_id_t create_next_variable_id_() {
        return variable_id_++;
    }

    env_id_t                              environment_id_;
    var_id_t                              variable_id_;
    std::unordered_map<SEXP, Environment> environment_mapping_;

  public:
    void resume_execution_timer() {
        execution_resume_time_ = std::chrono::high_resolution_clock::now();
    }

    void pause_execution_timer() {
        auto execution_pause_time = std::chrono::high_resolution_clock::now();
        std::uint64_t execution_time =
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                execution_pause_time - execution_resume_time_)
                .count();
        for (auto& element: stack_) {
            if (element.is_promise()) {
                element.get_promise()->add_execution_time(execution_time);
            }
        }
    }

  private:
    std::chrono::time_point<std::chrono::high_resolution_clock>
        execution_resume_time_;

    /***************************************************************************
     * PROMISE
     **************************************************************************/

  public:
    DenotedValue* create_promise(const SEXP promise) {
        DenotedValue* promise_state(create_raw_promise_(promise, true));
        auto result = promises_.insert_or_assign(promise, promise_state);
        promise_state->set_creation_timestamp(get_current_timestamp_());
        return promise_state;
    }

    DenotedValue* lookup_promise(const SEXP promise,
                                 bool       create = false,
                                 bool       local  = false) {
        auto iter = promises_.find(promise);

        /* all promises encountered are added to the map. Its not possible for
           a promise id to be encountered which is not already mapped.
           If this happens, possibly, the mapper methods are not the first to
           be called in the analysis. Hence, they are not able to update the
           mapping. */
        if (iter == promises_.end()) {
            if (create) {
                DenotedValue* promise_state(
                    create_raw_promise_(promise, local));
                promises_.insert({promise, promise_state});
                return promise_state;
            } else {
                return nullptr;
            }
        }

        return iter->second;
    }

    void remove_promise(const SEXP promise, DenotedValue* promise_state) {
        promises_.erase(promise);
        destroy_promise(promise_state);
    }

    void destroy_promise(DenotedValue* promise_state) {
        /* here we delete a promise iff we are the only one holding a
           reference to it. A promise can be simultaneously held by
           a call and this promise map. While it is held by the promise
           map, the active flag is set and while it is held by the call
           the argument flag is set. So, to decide if we have to delete
           the promise at this point, we first unset the active flag
           (because we are interesting in removing the promise) and then,
           we check the argument flag. If argument flag is unset, it means
           the promise is not held by a call and can be deleted. If the
           argument flag is set, it means the promise is held by a call
           and when that call gets deleted, it will delete this promise */
        promise_state->set_inactive();

        serialize_promise_(promise_state);

        summarize_promise_lifecycle_(promise_state->get_lifecycle());

        if (promise_state->has_escaped()) {
            serialize_escaped_promise_(promise_state);
        }

        if (!promise_state->is_argument()) {
            delete promise_state;
        }
    }

  private:
    denoted_value_id_t get_next_denoted_value_id_() {
        return denoted_value_id_counter_++;
    }

    void serialize_promise_(DenotedValue* promise) {
        promise_data_table_->write_row(
            promise->get_id(),
            promise->was_argument(),
            sexptype_to_string(promise->get_expression_type()),
            sexptype_to_string(promise->get_value_type()),
            promise->get_scope(),
            promise->get_S3_dispatch_count(),
            promise->get_S4_dispatch_count(),
            promise->is_preforced(),
            promise->get_force_count(),
            promise->get_evaluation_depth().call_depth,
            promise->get_evaluation_depth().promise_depth,
            promise->get_evaluation_depth().nested_promise_depth,
            promise->get_metaprogram_count(),
            promise->get_value_lookup_count(),
            promise->get_value_assign_count(),
            promise->get_expression_lookup_count(),
            promise->get_expression_assign_count(),
            promise->get_environment_lookup_count(),
            promise->get_environment_assign_count(),
            promise->get_self_scope_mutation_count(true),
            promise->get_self_scope_mutation_count(false),
            promise->get_lexical_scope_mutation_count(true),
            promise->get_lexical_scope_mutation_count(false),
            promise->get_non_lexical_scope_mutation_count(true),
            promise->get_non_lexical_scope_mutation_count(false),
            promise->get_self_scope_observation_count(true),
            promise->get_self_scope_observation_count(false),
            promise->get_lexical_scope_observation_count(true),
            promise->get_lexical_scope_observation_count(false),
            promise->get_non_lexical_scope_observation_count(true),
            promise->get_non_lexical_scope_observation_count(false),
            promise->get_execution_time());
    }

    void serialize_escaped_promise_(DenotedValue* promise) {
        escaped_argument_data_table_->write_row(
            promise->get_previous_call_id(),
            promise->get_previous_function_id(),
            sexptype_to_string(promise->get_previous_call_return_value_type()),
            promise->get_previous_formal_parameter_count(),
            promise->get_previous_formal_parameter_position(),
            promise->get_previous_actual_argument_position(),
            promise->get_id(),
            promise->get_class_name(),
            promise->get_S3_dispatch_count(),
            promise->get_S4_dispatch_count(),
            sexptype_to_string(promise->get_type()),
            sexptype_to_string(promise->get_expression_type()),
            sexptype_to_string(promise->get_value_type()),
            promise->get_previous_default_argument(),
            promise->does_non_local_return(),
            promise->has_escaped(),
            promise->get_evaluation_depth().call_depth,
            promise->get_evaluation_depth().promise_depth,
            promise->get_evaluation_depth().nested_promise_depth,
            promise->get_evaluation_depth().forcing_actual_argument_position,
            promise->is_preforced(),
            promise->get_force_count_before_escape(),
            promise->get_metaprogram_count_before_escape(),
            promise->get_value_lookup_count_before_escape(),
            promise->get_value_assign_count_before_escape(),
            promise->get_expression_lookup_count_before_escape(),
            promise->get_expression_assign_count_before_escape(),
            promise->get_environment_lookup_count_before_escape(),
            promise->get_environment_assign_count_before_escape(),
            promise->get_force_count_after_escape(),
            promise->get_metaprogram_count_after_escape(),
            promise->get_value_lookup_count_after_escape(),
            promise->get_value_assign_count_after_escape(),
            promise->get_expression_lookup_count_after_escape(),
            promise->get_expression_assign_count_after_escape(),
            promise->get_environment_lookup_count_after_escape(),
            promise->get_environment_assign_count_after_escape(),
            promise->get_self_scope_mutation_count_before_escape(true),
            promise->get_self_scope_mutation_count_before_escape(false),
            promise->get_lexical_scope_mutation_count_before_escape(true),
            promise->get_lexical_scope_mutation_count_before_escape(false),
            promise->get_non_lexical_scope_mutation_count_before_escape(true),
            promise->get_non_lexical_scope_mutation_count_before_escape(false),
            promise->get_self_scope_observation_count_before_escape(true),
            promise->get_self_scope_observation_count_before_escape(false),
            promise->get_lexical_scope_observation_count_before_escape(true),
            promise->get_lexical_scope_observation_count_before_escape(false),
            promise->get_non_lexical_scope_observation_count_before_escape(
                true),
            promise->get_non_lexical_scope_observation_count_before_escape(
                false),
            promise->get_self_scope_mutation_count_after_escape(true),
            promise->get_self_scope_mutation_count_after_escape(false),
            promise->get_lexical_scope_mutation_count_after_escape(true),
            promise->get_lexical_scope_mutation_count_after_escape(false),
            promise->get_non_lexical_scope_mutation_count_after_escape(true),
            promise->get_non_lexical_scope_mutation_count_after_escape(false),
            promise->get_self_scope_observation_count_after_escape(true),
            promise->get_self_scope_observation_count_after_escape(false),
            promise->get_lexical_scope_observation_count_after_escape(true),
            promise->get_lexical_scope_observation_count_after_escape(false),
            promise->get_non_lexical_scope_observation_count_after_escape(true),
            promise->get_non_lexical_scope_observation_count_after_escape(
                false),
            promise->get_execution_time());
    }

    DenotedValue* create_raw_promise_(const SEXP promise, bool local) {
        const SEXP    rho    = dyntrace_get_promise_environment(promise);
        env_id_t      env_id = lookup_environment(rho).get_id();
        DenotedValue* promise_state =
            new DenotedValue(get_next_denoted_value_id_(), promise, local);

        set_scope_(promise_state);

        /* Setting this bit tells us that the promise is currently in the
           promises table. As long as this is set, the call holding a reference
           to it will not delete it. */
        promise_state->set_active();
        return promise_state;
    }

    void set_scope_(DenotedValue* denoted_value) {
        auto& stack = get_stack();
        for (auto iter = stack.rbegin(); iter != stack.rend(); ++iter) {
            /* we do not stop at first call because in all cases it turns out to
               be
               '{' function. We want to keep going back until we find a closure
             */
            if (iter->is_call()) {
                denoted_value->set_scope(iter->get_call()->get_function_id());
            }
            if (iter->is_closure()) {
                break;
            }
        }
    }

    std::unordered_map<SEXP, DenotedValue*> promises_;
    denoted_value_id_t                      denoted_value_id_counter_;

  private:
    timestamp_t get_current_timestamp_() const {
        return timestamp_;
    }

    timestamp_t increment_timestamp_() {
        return timestamp_++;
    }

    timestamp_t timestamp_;

  public:
    void exit_probe() {
        resume_execution_timer();
    }

    void enter_probe() {
        pause_execution_timer();
        increment_timestamp_();
    }

  public:
    Call* create_call(const SEXP call,
                      const SEXP op,
                      const SEXP args,
                      const SEXP rho) {
        Function*            function      = lookup_function(op);
        Call*                function_call = nullptr;
        call_id_t            call_id       = get_next_call_id_();
        const function_id_t& function_id   = function->get_id();
        const std::string    function_name = get_name(call);
        int                  eval          = 0;

        function_call = new Call(call_id,
                                 function_id,
                                 TYPEOF(op),
                                 function_name,
                                 function->get_formal_parameter_count(),
                                 rho,
                                 function);

        if (TYPEOF(op) == CLOSXP) {
            process_closure_arguments_(function_call, op);
        } else {
            int eval = dyntrace_get_c_function_argument_evaluation(op);
            function_call->set_force_order(eval);
        }

        return function_call;
    }

    void destroy_call(Call* call) {
        Function* function = call->get_function();

        function->add_summary(call);

        for (Argument* argument: call->get_arguments()) {
            serialize_argument_(argument);

            DenotedValue* value = argument->get_denoted_value();

            if (!value->is_active()) {
                delete value;
            } else {
                value->remove_argument(call->get_id(),
                                       call->get_function_id(),
                                       call->get_return_value_type(),
                                       call->get_formal_parameter_count(),
                                       argument);
            }

            argument->set_denoted_value(nullptr);

            delete argument;
        }

        delete call;
    }

  private:
    call_id_t get_next_call_id_() {
        return ++call_id_counter_;
    }

    void process_closure_argument_(Call*      call,
                                   int        formal_parameter_position,
                                   int        actual_argument_position,
                                   const SEXP name,
                                   const SEXP argument,
                                   bool       dot_dot_dot) {
        DenotedValue* value = nullptr;
        /* only add to promise map if the argument is a promise */
        if (type_of_sexp(argument) == PROMSXP) {
            value = lookup_promise(argument, true);
        } else {
            value =
                new DenotedValue(get_next_denoted_value_id_(), argument, false);
            set_scope_(value);
        }

        bool default_argument = true;

        if (value->is_promise()) {
            default_argument =
                call->get_environment() == value->get_environment();
        }

        Argument* arg = new Argument(call,
                                     formal_parameter_position,
                                     actual_argument_position,
                                     default_argument,
                                     dot_dot_dot);
        arg->set_denoted_value(value);

        value->add_argument(arg);

        call->add_argument(arg);
    }

    void process_closure_arguments_(Call* call, const SEXP op) {
        SEXP formal                    = nullptr;
        SEXP name                      = nullptr;
        SEXP argument                  = nullptr;
        SEXP dot_dot_dot_arguments     = nullptr;
        SEXP rho                       = call->get_environment();
        int  formal_parameter_position = -1;
        int  actual_argument_position  = -1;

        for (formal = FORMALS(op); formal != R_NilValue; formal = CDR(formal)) {
            ++formal_parameter_position;

            /* get argument name */
            name = TAG(formal);
            /* lookup argument in environment by name */
            argument = dyntrace_lookup_environment(rho, name);

            switch (type_of_sexp(argument)) {
            case DOTSXP:
                for (SEXP dot_dot_dot_arguments = argument;
                     dot_dot_dot_arguments != R_NilValue;
                     dot_dot_dot_arguments = CDR(dot_dot_dot_arguments)) {
                    ++actual_argument_position;

                    name = TAG(dot_dot_dot_arguments);

                    SEXP dot_dot_dot_argument = CAR(dot_dot_dot_arguments);

                    process_closure_argument_(call,
                                              formal_parameter_position,
                                              actual_argument_position,
                                              name,
                                              dot_dot_dot_argument,
                                              true);
                }
                break;

            default:
                ++actual_argument_position;
                process_closure_argument_(call,
                                          formal_parameter_position,
                                          actual_argument_position,
                                          name,
                                          argument,
                                          is_dots_symbol(name));
                break;
            }
        }
    }

    void serialize_argument_(Argument* argument) {
        Call*         call  = argument->get_call();
        DenotedValue* value = argument->get_denoted_value();

        argument_data_table_->write_row(
            call->get_id(),
            call->get_function_id(),
            value->get_id(),
            argument->get_formal_parameter_position(),
            argument->get_actual_argument_position(),
            sexptype_to_string(value->get_type()),
            sexptype_to_string(value->get_expression_type()),
            sexptype_to_string(value->get_value_type()),
            argument->is_default_argument(),
            argument->is_dot_dot_dot(),
            value->is_preforced(),
            argument->is_directly_forced(),
            argument->get_direct_lookup_count(),
            argument->get_direct_metaprogram_count(),
            argument->is_indirectly_forced(),
            argument->get_indirect_lookup_count(),
            argument->get_indirect_metaprogram_count(),
            argument->used_for_S3_dispatch(),
            argument->used_for_S4_dispatch(),
            argument->get_forcing_actual_argument_position(),
            argument->does_non_local_return());
        // value->has_escaped(),
        // value->get_evaluation_depth().call_depth,
        // value->get_evaluation_depth().promise_depth,
        // value->get_evaluation_depth().nested_promise_depth,
        // value->get_evaluation_depth().forcing_actual_argument_position,
        // value->is_preforced(), value->get_force_count(),
        // value->get_value_lookup_count(),
        // value->get_value_assign_count(),
        // value->get_expression_lookup_count(),
        // value->get_expression_assign_count(),
        // value->get_environment_lookup_count(),
        // value->get_environment_assign_count(),
        // value->get_self_scope_mutation_count(true),
        // value->get_self_scope_mutation_count(false),
        // value->get_lexical_scope_mutation_count(true),
        // value->get_lexical_scope_mutation_count(false),
        // value->get_non_lexical_scope_mutation_count(true),
        // value->get_non_lexical_scope_mutation_count(false),
        // value->get_self_scope_observation_count(true),
        // value->get_self_scope_observation_count(false),
        // value->get_lexical_scope_observation_count(true),
        // value->get_lexical_scope_observation_count(false),
        // value->get_non_lexical_scope_observation_count(true),
        // value->get_non_lexical_scope_observation_count(false),
        // value->get_execution_time());
    }

    DataTableStream* argument_data_table_;
    DataTableStream* escaped_argument_data_table_;

    /***************************************************************************
     * Function API
     ***************************************************************************/
  public:
    Function* lookup_function(const SEXP op) {
        auto iter = functions_.find(op);
        if (iter == functions_.end()) {
            Function* function = new Function(op);
            functions_.insert({op, function});
            return function;
        }
        return iter->second;
    }

    void remove_function(const SEXP op) {
        auto it = functions_.find(op);

        if (it != functions_.end()) {
            destroy_function_(it->second);
            functions_.erase(it);
        }
    }

  private:
    void destroy_function_(Function* function) {
        serialize_function_(function);
        delete function;
    }

    DataTableStream*                    call_summary_data_table_;
    DataTableStream*                    function_definition_data_table_;
    std::unordered_set<function_id_t>   serialized_functions_;
    std::unordered_map<SEXP, Function*> functions_;

    void serialize_function_(Function* function) {
        serialize_function_call_summary_(*function);
        serialize_function_definition_(*function);
    }

    void serialize_function_call_summary_(const Function& function) {
        const std::string& function_namespace = function.get_namespace();
        const std::vector<std::string> function_names = function.get_names();

        std::string all_names = "";

        if (function_names.size() >= 1) {
            all_names = function_namespace + "::" + function_names[0];
        }

        for (std::size_t i = 1; i < function_names.size(); ++i) {
            all_names += " | " + function_namespace + "::" + function_names[i];
        }

        for (std::size_t i = 0; i < function.get_summary_count(); ++i) {
            call_summary_data_table_->write_row(
                function.get_id(),
                sexptype_to_string(function.get_type()),
                function.get_formal_parameter_count(),
                function.is_wrapper(),
                all_names,
                function.get_generic_method_name(),
                function.is_dispatcher(),
                pos_seq_to_string(function.get_force_order(i)),
                pos_seq_to_string(function.get_missing_arguments(i)),
                sexptype_to_string(function.get_return_value_type(i)),
                function.get_call_count(i));
        }
    }

    void serialize_function_definition_(const Function& function) {
        auto res = serialized_functions_.insert(function.get_id());

        /* serialize function definition iff the insertion was successful,
           i.e. the function was not previously serialized. */
        if (res.second) {
            function_definition_data_table_->write_row(
                function.get_id(),
                function.is_byte_compiled(),
                function.get_definition());
        }
    }

  public:
    void identify_side_effect_creators(const Variable& var, const SEXP env) {
        bool                   direct = true;
        ExecutionContextStack& stack  = get_stack();

        for (auto iter = stack.rbegin(); iter != stack.rend(); ++iter) {
            ExecutionContext& exec_ctxt = *iter;

            if (exec_ctxt.is_closure()) {
                if (exec_ctxt.get_closure()->get_environment() == env) {
                    /* its normal for a function to mutate variables in its
                       own environment. this case is not interesting. */
                    return;
                }
            }

            if (exec_ctxt.is_promise()) {
                DenotedValue* promise = exec_ctxt.get_promise();

                const SEXP prom_env = promise->get_environment();

                if (prom_env == env) {
                    const timestamp_t var_timestamp =
                        var.get_modification_timestamp();
                    if (promise->get_creation_timestamp() > var_timestamp) {
                        promise->set_self_scope_mutation(direct);
                        direct = false;
                        return;
                    }
                } else if (is_parent_environment(env, prom_env)) {
                    /* if this happens, promise is causing side effect
                       in its lexically scoped environment. */
                    promise->set_lexical_scope_mutation(direct);
                    direct = false;
                } else {
                    /* if this happens, promise is causing side effect
                       in non lexically scoped environment */
                    promise->set_non_lexical_scope_mutation(direct);
                    direct = false;
                }
            }
        }
    }

    void identify_side_effect_observers(const Variable& var, const SEXP env) {
        const timestamp_t var_timestamp = var.get_modification_timestamp();

        /* this implies we have not seen the variable before.
           TODO - see when this case is triggered and ensure
           there is no bug */
        if (timestamp_is_undefined(var_timestamp)) {
            return;
        }

        /* if the modification timestamp of the variable is
           greater than the creation timestamp of the promise,
           then, that promise has identified a side effect */

        bool direct = true;

        ExecutionContextStack& stack = get_stack();

        for (auto iter = stack.rbegin(); iter != stack.rend(); ++iter) {
            ExecutionContext& exec_ctxt = *iter;

            /* If the most recent context that is responsible for this
               side effect is a closure, then return. Currently, we only
               care about promises directly responsible for side effects.
               We don't return in case of specials and builtins because
               they are more like operators in a programming language and
               everything ultimately happens in them and returning on
               encountering them will make it look like no promise has
               caused a side effect. */

            if (exec_ctxt.is_closure()) {
                if (exec_ctxt.get_closure()->get_environment() == env) {
                    /* its normal for a function to mutate variables in its
                       own environment. this case is not interesting. */
                    return;
                }
            }

            if (exec_ctxt.is_promise()) {
                DenotedValue* promise = exec_ctxt.get_promise();

                const SEXP prom_env = promise->get_environment();

                if (prom_env == env) {
                    const timestamp_t var_timestamp =
                        var.get_modification_timestamp();
                    if (promise->get_creation_timestamp() < var_timestamp) {
                        promise->set_self_scope_observation(direct);
                        direct = false;
                        return;
                    }
                } else if (is_parent_environment(env, prom_env)) {
                    /* if this happens, promise is causing side effect
                       in its lexically scoped environment. */
                    promise->set_lexical_scope_observation(direct);
                    direct = false;
                } else {
                    /* if this happens, promise is observing side effect
                       in non lexically scoped environment */
                    promise->set_non_lexical_scope_observation(direct);
                    direct = false;
                }
            }
        }
    }

    /* is env_a parent of env_b? */
    bool is_parent_environment(SEXP env_a, SEXP env_b) {
        SEXP env_cur = R_NilValue;
        if (env_a == env_b)
            return false;
        for (env_cur = ENCLOS(env_b); env_cur != R_NilValue;
             env_cur = ENCLOS(env_cur)) {
            if (env_cur == env_a)
                return true;
        }
        return false;
    }

    void update_wrapper_state(Call* call) {
        ExecutionContextStack& stack = get_stack();

        if (!stack.is_empty()) {
            ExecutionContext exec_ctxt = stack.peek(1);

            if (exec_ctxt.is_closure()) {
                Function* called_function = call->get_function();
                Function* caller_function =
                    exec_ctxt.get_closure()->get_function();
                caller_function->update_wrapper(
                    called_function->is_internal() ||
                    called_function->is_primitive());
            }
        }
    }

    eval_depth_t get_evaluation_depth(Call* call) {
        ExecutionContextStack&                  stack = get_stack();
        ExecutionContextStack::reverse_iterator iter;
        eval_depth_t                            eval_depth = {0, 0, 0, -1};
        bool                                    nesting    = true;

        for (iter = stack.rbegin(); iter != stack.rend(); ++iter) {
            ExecutionContext& exec_ctxt = *iter;

            if (exec_ctxt.is_closure()) {
                nesting = false;
                if (exec_ctxt.get_call() == call)
                    break;
                ++eval_depth.call_depth;
            } else if (exec_ctxt.is_promise()) {
                ++eval_depth.promise_depth;
                if (nesting)
                    ++eval_depth.nested_promise_depth;
                DenotedValue* promise = exec_ctxt.get_promise();
                if (eval_depth.forcing_actual_argument_position == -1 &&
                    promise->is_argument() &&
                    promise->get_last_argument()->get_call() == call) {
                    eval_depth.forcing_actual_argument_position =
                        promise->get_last_argument()
                            ->get_actual_argument_position();
                }
            }
        }

        // if this happens, it means we could not locate the call from which
        // this promise originated. This means that this is an escaped
        // promise.
        if (iter == stack.rend()) {
            return ESCAPED_PROMISE_EVAL_DEPTH;
        }

        return eval_depth;
    }

    void summarize_promise_lifecycle_(const lifecycle_t& lifecycle) {
        for (std::size_t i = 0; i < lifecycle_summary_.size(); ++i) {
            if (lifecycle_summary_[i].first.action == lifecycle.action &&
                lifecycle_summary_[i].first.count == lifecycle.count) {
                ++lifecycle_summary_[i].second;
                return;
            }
        }
        lifecycle_summary_.push_back({lifecycle, 1});
    }

    void serialize_promise_lifecycle_summary_() {
        for (const auto& summary: lifecycle_summary_) {
            promise_lifecycle_data_table_->write_row(
                summary.first.action,
                pos_seq_to_string(summary.first.count),
                summary.second);
        }
    }

  private:
    call_id_t                                call_id_counter_;
    std::vector<unsigned int>                object_count_;
    std::vector<std::pair<lifecycle_t, int>> lifecycle_summary_;
};

#endif /* PROMISEDYNTRACER_TRACER_STATE_H */