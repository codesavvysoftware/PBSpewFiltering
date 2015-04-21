
#ifndef RAD_PRETTYOPTIONPRINTER_HPP
#define RAD_PRETTYOPTIONPRINTER_HPP

#include "CustomOptionDescription.hpp"

namespace rad
{
//*********************************************************************************************************************
  class OptionPrinter
  {    
  public: // interface
    void addOption(const CustomOptionDescription& optionDesc);

    /** Print the single line application usage description */
    std::string usage();
    std::string usage_withoutRequiredOptionDesc();

    std::string positionalOptionDetails();
    std::string optionDetails();

  public: // static
    static void printStandardAppDesc(const std::string& appName,
                                     std::ostream& out,
                                     boost::program_options::options_description desc,
                                     boost::program_options::positional_options_description* positionalDesc=NULL);
    static void formatRequiredOptionError(boost::program_options::required_option& error);

  private: // data
    std::vector<CustomOptionDescription> options_;
    std::vector<CustomOptionDescription> positionalOptions_;

  }; // class

//*********************************************************************************************************************

} // namespace

#endif // RAD_PRETTYOPTIONPRINTER_HPP
