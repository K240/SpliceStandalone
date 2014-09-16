#
# Copyright 2010-2013 Fabric Engine Inc. All rights reserved.
#

import os, sys, platform, copy

Import('parentEnv', 'FABRIC_CAPI_DIR', 'FABRIC_SPLICE_VERSION', 'STAGE_DIR', 'FABRIC_BUILD_OS', 'FABRIC_BUILD_TYPE', 'QT_INCLUDE_DIR', 'QT_LIB_DIR', 'sharedCapiFlags', 'spliceFlags')

qtDir = os.path.split(QT_INCLUDE_DIR)[0]
if FABRIC_BUILD_OS == 'Linux':
  qtDir = '/usr'

# create the build environment
env = Environment(tools=['default','qt'], QTDIR=qtDir, QT_LIB='', ENV=parentEnv['ENV'])

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

libSuffix = ''
if FABRIC_BUILD_OS == 'Windows':
  libSuffix = '4'
  if FABRIC_BUILD_TYPE == 'Debug':
    libSuffix = 'd' + libSuffix
if FABRIC_BUILD_OS == 'Windows':
  qtFlags['CCFLAGS'] = ['/DNT_PLUGIN']
elif FABRIC_BUILD_OS == 'Linux':
  qtFlags['CCFLAGS'] = ['-DLINUX']

qtFlags['LIBS'] = ['QtCore'+libSuffix, 'QtGui'+libSuffix, 'QtOpenGL'+libSuffix]

if FABRIC_BUILD_OS == 'Windows':
  env.Append(LIBS = ['advapi32', 'shell32', 'user32', 'Opengl32', 'glu32', 'gdi32'])
elif FABRIC_BUILD_OS == 'Linux':
  env.Append(LIBS = ['boost_program_options'])

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
sources += env.Glob('src/Widgets/*.cpp')
sources += env.Glob('src/Widgets/AE/*.cpp')

target = 'FabricSpliceStandalone' + FABRIC_SPLICE_VERSION

if FABRIC_BUILD_OS == 'Linux':
  env[ '_LIBFLAGS' ] = '-Wl,--start-group ' + env['_LIBFLAGS'] + ' -Wl,--end-group'
elif FABRIC_BUILD_OS == 'Windows':
  env.Append(LINKFLAGS = ['/STACK:67108864'])

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

# install the extensions
for ext in ['SpliceStandalone']:
  extFiles = env.GlobRecursive(os.path.join(env.Dir('.').abspath, 'Exts', ext, '*.kl'))
  extFiles += env.GlobRecursive(os.path.join(env.Dir('.').abspath, 'Exts', ext, '*.json'))
  for extFile in extFiles:
    absFile = extFile.srcnode().abspath
    relFile = os.path.relpath(absFile, env.Dir('.').srcnode().abspath)
    absFile = os.path.join(STAGE_DIR.abspath, relFile)
    standaloneFiles.append(env.Install(os.path.split(absFile)[0], extFile))
standaloneFiles.append(env.Install(STAGE_DIR.Dir('images'), env.Glob(os.path.join(env.Dir('.').abspath, 'images', '*'))))
  
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
