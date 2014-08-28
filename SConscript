#
# Copyright 2010-2013 Fabric Engine Inc. All rights reserved.
#

import os, sys, platform, copy

Import('parentEnv', 'FABRIC_CAPI_DIR', 'FABRIC_SPLICE_VERSION', 'STAGE_DIR', 'FABRIC_BUILD_OS', 'FABRIC_BUILD_TYPE', 'QT_INCLUDE_DIR', 'QT_LIB_DIR', 'sharedCapiFlags', 'spliceFlags')

# create the build environment
env = Environment(tools=['default','qt'], QTDIR=os.path.split(QT_INCLUDE_DIR)[0], QT_LIB='')

env.Append(CCFLAGS = parentEnv['CCFLAGS'])
env.Append(LINKFLAGS = parentEnv['LINKFLAGS'])
env.Append(CPPDEFINES = parentEnv['CPPDEFINES'])
env.Append(CPPPATH = parentEnv['CPPPATH'])
env.Append(LIBPATH = parentEnv['LIBPATH'])
env.Append(LIBS = parentEnv['LIBS'])

qtFlags = {
  'CPPPATH': [
      QT_INCLUDE_DIR,
      os.path.join(QT_INCLUDE_DIR, 'Qt')
    ],
  'LIBPATH': [
    QT_LIB_DIR
  ]
}

if FABRIC_BUILD_OS == 'Windows':
  qtFlags['CCFLAGS'] = ['/DNT_PLUGIN']
  qtFlags['LIBS'] = ['QtCore4', 'QtGui4', 'QtOpenGL4']
elif buildOS == 'Linux':
  qtFlags['CCFLAGS'] = ['-DLINUX']
  qtFlags['LIBS'] = ['QtCore', 'QtGui', 'QtOpenGL']
else:
  qtFlags['LIBS'] = ['QtCore', 'QtGui', 'QtOpenGL']

if FABRIC_BUILD_OS == 'Windows':
  env.Append(LIBS = ['advapi32', 'shell32', 'user32', 'Opengl32', 'glu32', 'gdi32'])

env.MergeFlags(qtFlags)
env.MergeFlags(sharedCapiFlags)
env.MergeFlags(spliceFlags)

env.Append(CPPPATH = [env.Dir('src')])

def GlobRecursive(self, pattern, useBuildDir = False):
  (folder, file) = os.path.split(pattern)
  if not folder:
    folder = '.'

  if not os.path.isabs(folder):
    folder = self.Dir(folder).srcnode().abspath

  if useBuildDir:
    if os.path.isabs(folder):
      pattern = os.path.join(os.path.relpath(folder, env.Dir('.').srcnode().abspath), file)

  result = Glob(pattern)
  for root, dirnames, filenames in os.walk(folder):
    doneDir = {'.': True}
    for dirname in dirnames:
      if doneDir.has_key(dirname):
        continue
      doneDir[dirname] = True
      result += self.GlobRecursive(os.path.join(root, dirname, file), useBuildDir = useBuildDir)


  result = self.Flatten(result)
  return result
env.AddMethod(GlobRecursive)

sources = env.Glob('src/*.cpp')
sources += env.GlobRecursive('src/Widgets/*.cpp', useBuildDir = True)

target = 'FabricSpliceStandalone' + FABRIC_SPLICE_VERSION

standaloneFiles = []
standaloneApp = env.Program(target, sources)
installedApp = env.Install(STAGE_DIR, standaloneApp)

standaloneFiles.append(installedApp)
standaloneFiles.append(env.Install(STAGE_DIR, env.File('license.txt')))

# also install the FabricCore dynamic library
standaloneFiles.append(env.Install(STAGE_DIR, env.Glob(os.path.join(FABRIC_CAPI_DIR, 'lib', '*.so'))))
standaloneFiles.append(env.Install(STAGE_DIR, env.Glob(os.path.join(FABRIC_CAPI_DIR, 'lib', '*.dylib'))))
standaloneFiles.append(env.Install(STAGE_DIR, env.Glob(os.path.join(FABRIC_CAPI_DIR, 'lib', '*.dll'))))

# also install the qt libs
if FABRIC_BUILD_OS == 'Windows':
  qtBinDir = os.path.join(os.path.split(QT_LIB_DIR)[0], 'bin')
  for qtLib in qtFlags['LIBS']:
    standaloneFiles.append(env.Install(STAGE_DIR, env.Glob(os.path.join(qtBinDir, '*%s*.so' % qtLib))))
    standaloneFiles.append(env.Install(STAGE_DIR, env.Glob(os.path.join(qtBinDir, '*%s*.dylib' % qtLib))))
    standaloneFiles.append(env.Install(STAGE_DIR, env.Glob(os.path.join(qtBinDir, '*%s*.dll' % qtLib))))

# install PDB files on windows
if FABRIC_BUILD_TYPE == 'Debug' and FABRIC_BUILD_OS == 'Windows':
  env['CCPDBFLAGS']  = ['${(PDB and "/Fd%s_incremental.pdb /Zi" % File(PDB)) or ""}']
  pdbSource = standaloneApp[0].get_abspath().rpartition('.')[0]+".pdb"
  pdbTarget = os.path.join(STAGE_DIR.abspath, os.path.split(pdbSource)[1])
  copyPdb = env.Command( 'copy', None, 'copy "%s" "%s" /Y' % (pdbSource, pdbTarget) )
  env.Depends( copyPdb, installedApp )
  env.AlwaysBuild(copyPdb)

alias = env.Alias('splicestandalone', standaloneFiles)
spliceData = (alias, standaloneFiles)
Return('spliceData')
