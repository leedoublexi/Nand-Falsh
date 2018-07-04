import os
import CopyTree
import glob

# Import our environment defiend in the SConstruct.  Go ahead and clone it so we don't destroy the original.
Import('env')
if not env['cpu'] or env['name'].startswith('sdk_'):
	Return()

#as a hack-around, this project is intended to be located at /LF/Base/APP_NAME and not /LF/Bulk/ProgramFiles/APP_NAME
install_org=env['install_dir'].abspath
env['install_dir']=Dir(install_org.replace('LF/Bulk/ProgramFiles','LF/Base'));
print env['install_dir']
myEnv = env.Clone()

# Define what we want to call out executable
appName = 'CartridgeWriter'


# Define where our source files are.
sources = Glob('*.cpp') + Glob('*.c')


# Define where our header files are.
myEnv.Append(CPPPATH = ['Include'])


# Define what libs we want to include.
# myEnv.Append(LIBPATH = ['#/libs'])
# myEnv.Append(LIBS = ['AccelerometerMPI', 'ControllerMPI', 'LightningBase', 'LightningJSON', 'screenshot', 'ImageIO', 'pthread', 'png'])
#For Rio
#myEnv.Append(LIBS = ['AccelerometerMPI', 'LightningBase', 'LightningJSON', 'screenshot', 'ImageIO', 'pthread', 'png'])


# Optimizer stuff needed to allow exceptions to work.  Needs to be revisted if we get gcc 4.3 or later.
if env['cpu'] != 'x86':
	myEnv.Append(CCFLAGS = ['-fno-omit-frame-pointer'])


# Build the application and install it to the proper directory.
myModule = myEnv.Program(appName, sources, SHLIBPREFIX='')
env['targets'] += env.Install( env['install_dir'], myModule)
