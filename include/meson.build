###########
# headers #
###########

if not meson.is_subproject()
  install_headers(
    files([
      'pacbio/trima/char_traits.hpp',
      'pacbio/trima/fasta.hpp',
      'pacbio/trima/fastq.hpp',
      'pacbio/trima/format.hpp',
      'pacbio/trima/hmm_model.hpp',
      'pacbio/trima/hmm_utilities.h',
      'pacbio/trima/kernel_color.h',
      'pacbio/trima/matrix.hpp',
      'pacbio/trima/polyA_hmm_model.hpp',
      'pacbio/trima/quality.hpp',
      'pacbio/trima/sequence.hpp',
      'pacbio/trima/thread.hpp',
      'pacbio/trima/type_policy.h']),
    subdir : 'pacbio/trima')
endif

trima_include_directories = include_directories('.')
