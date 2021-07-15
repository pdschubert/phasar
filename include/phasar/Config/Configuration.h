/******************************************************************************
 * Copyright (c) 2017 Philipp Schubert.
 * All rights reserved. This program and the accompanying materials are made
 * available under the terms of LICENSE.txt.
 *
 * Contributors:
 *     Philipp Schubert and others
 *****************************************************************************/

/*
 * Configuration.hh
 *
 *  Created on: 04.05.2017
 *      Author: philipp
 */

#ifndef PHASAR_CONFIG_CONFIGURATION_H_
#define PHASAR_CONFIG_CONFIGURATION_H_

#include <string>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

#include "llvm/ADT/iterator_range.h"
#include "llvm/Support/ManagedStatic.h"

#include "phasar/Config/Version.h"

#define XSTR(S) STR(S)
#define STR(S) #S

#if __cplusplus >= 202002L
#define PSR_CONSTEXPR constexpr
#define PSR_CONST_CONSTEXPR constexpr
#else
#define PSR_CONSTEXPR
#define PSR_CONST_CONSTEXPR const
#endif

namespace psr {

class PhasarConfig {
public:
  /// Current Phasar version
  static PSR_CONSTEXPR std::string PhasarVersion() {
    return XSTR(PHASAR_VERSION);
  }

  /// Stores the label/ tag with which we annotate the LLVM IR.
  static PSR_CONSTEXPR std::string MetaDataKind() { return "psr.id"; }

  static PSR_CONSTEXPR std::string ConfigurationDirectory() {
    return configuration_directory;
  }

  /// Specifies the directory in which Phasar is located.
  static PSR_CONSTEXPR std::string PhasarDirectory() { return PhasarDir; }

  /// Name of the file storing all standard header search paths used for
  /// compilation.
  static PSR_CONSTEXPR std::string HeaderSearchPathsFileName() {
    return "standard_header_paths.conf";
  }

  /// Name of the compile_commands.json file (in case we wish to rename)
  static PSR_CONSTEXPR std::string CompileCommandsJson() {
    return "compile_commands.json";
  }

  /// Default Source- and Sink-Functions path
  static PSR_CONSTEXPR std::string DefaultSourceSinkFunctionsPath() {
    return std::string(PhasarDirectory() +
                       "config/phasar-source-sink-function.json");
  }

  // Variables to be used in JSON export format
  /// Identifier for call graph export
  static PSR_CONSTEXPR std::string JsonCallGraphID() { return "CallGraph"; }

  /// Identifier for type hierarchy graph export
  static PSR_CONSTEXPR std::string JsonTypeHierarchyID() {
    return "TypeHierarchy";
  }

  /// Identifier for points-to graph export
  static PSR_CONSTEXPR std::string JsonPointsToGraphID() {
    return "PointsToGraph";
  }

  /// Identifier for data-flow results export
  static PSR_CONSTEXPR std::string JsonDataFlowID() {
    return "DataFlowInformation";
  }

  static PhasarConfig &getPhasarConfig();

  llvm::iterator_range<std::set<std::string>::iterator> specialFunctionNames() {
    return llvm::make_range(special_function_names.begin(),
                            special_function_names.end());
  }

  /// Add a function name to the special functions list.
  /// Special functions are functions that cannot directly be analyzed but need
  /// to be handled by the analysis.
  ///
  /// Remark: Manually added special functions need to be added before creating
  /// the analysis.
  void addSpecialFunctionName(std::string SFName) {
    special_function_names.insert(std::move(SFName));
  }

  /// Variables map of the parsed command-line parameters
  static boost::program_options::variables_map &VariablesMap() {
    static boost::program_options::variables_map variables_map;
    return variables_map;
  }

  ~PhasarConfig() = default;
  PhasarConfig(const PhasarConfig &) = delete;
  PhasarConfig(PhasarConfig &&) = delete;

private:
  PhasarConfig();

  static std::string readConfigFile(const std::string &path);
  void loadGlibcSpecialFunctionNames();
  void loadLLVMSpecialFunctionNames();

  std::set<std::string> special_function_names;

  /// Specifies the directory in which important configuration files are
  /// located.
  inline static const std::string configuration_directory = []() {
    auto *env_home = std::getenv("HOME");
    std::string config_folder = "config/";
    if (env_home) { // Check if HOME was defined in the environment
      std::string phasar_config = std::string(env_home) + "/.config/phasar/";
      if (boost::filesystem::exists(phasar_config) &&
          boost::filesystem::is_directory(phasar_config)) {
        config_folder = phasar_config;
      }
    }
    return config_folder;
  }();

  /// Specifies the directory in which Phasar is located.
  static const std::string PhasarDir;

  /// Name of the file storing all glibc function names.
  static inline PSR_CONST_CONSTEXPR std::string GLIBCFunctionListFileName =
      "glibc_function_list_v1-04.05.17.conf";

  /// Name of the file storing all LLVM intrinsic function names.
  static inline PSR_CONST_CONSTEXPR std::string
      LLVMIntrinsicFunctionListFileName =
          "llvm_intrinsics_function_list_v1-04.05.17.conf";

  /// Log file directory
  static inline PSR_CONST_CONSTEXPR std::string LogFileDirectory = "log/";
};

} // namespace psr

#endif
