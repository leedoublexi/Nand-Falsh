# ============================================================================
# Lf Aria Style Scons
# ----------------------------------------------------------------------------
# This needs the LF Aria SDK installed in order to work properly.  
# In addition you'll also need "site_scons" in the project directory.
# This a pretty generic container, all the real magic happens in SConscript
# ============================================================================

import EnvTargets
import os
import subprocess

# Update the meta.inf
# subprocess.call(['./updateMeta.sh'])

# Call forth the sconscript to do the real work.
envs = EnvTargets.GetStdEnvironments(ARGUMENTS, 'CartridgeWriter')
EnvTargets.IncludeSDK(envs, ARGUMENTS)
for target, env in envs.iteritems():
	Export('env')
	SConscript('SConscript', variant_dir=env['variant_dir'], duplicate=0)
EnvTargets.CreateTargets(envs)
