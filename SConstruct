#!/usr/bin/env python
import os
import sys

from methods import print_error


libname = "godot-syphon"
projectdir = "demo"

localEnv = Environment(tools=["default"], PLATFORM="")

# Build profiles can be used to decrease compile times.
# You can either specify "disabled_classes", OR
# explicitly specify "enabled_classes" which disables all other classes.
# Modify the example file as needed and uncomment the line below or
# manually specify the build_profile parameter when running SCons.

# localEnv["build_profile"] = "build_profile.json"

customs = ["custom.py"]
customs = [os.path.abspath(path) for path in customs]

opts = Variables(customs, ARGUMENTS)
opts.Update(localEnv)

Help(opts.GenerateHelpText(localEnv))

env = localEnv.Clone()

if not (os.path.isdir("godot-cpp") and os.listdir("godot-cpp")):
    print_error("""godot-cpp is not available within this folder, as Git submodules haven't been initialized.
Run the following command to download godot-cpp:

    git submodule update --init --recursive""")
    sys.exit(1)

env = SConscript("godot-cpp/SConstruct", {"env": env, "customs": customs})

env.Append(CPPPATH=["src/"])
sources = Glob("src/*.cpp") + Glob("src/*.mm") + Glob("src/*.m")

if env["target"] in ["editor", "template_debug"]:
    try:
        doc_data = env.GodotCPPDocData("src/gen/doc_data.gen.cpp", source=Glob("doc_classes/*.xml"))
        sources.append(doc_data)
    except AttributeError:
        print("Not including class reference as we're targeting a pre-4.3 baseline.")

# .dev doesn't inhibit compatibility, so we don't need to key it.
# .universal just means "compatible with all relevant arches" so we don't need to key it.
suffix = env['suffix'].replace(".dev", "").replace(".universal", "")

lib_filename = "{}{}{}{}".format(env.subst('$SHLIBPREFIX'), libname, suffix, env.subst('$SHLIBSUFFIX'))

library = env.SharedLibrary(
    "bin/{}/{}".format(env['platform'], lib_filename),
    source=sources,
)

env.Append(LIBPATH=[os.path.abspath(os.path.join(projectdir, "bin", env['platform']))])
env.Append(CXXFLAGS=" -I./src/third_party/Syphon.xcframework/macos-arm64_x86_64/Syphon.framework/Headers/")
env.Append(LINKFLAGS="-Fsrc/third_party/Syphon.xcframework/macos-arm64_x86_64/ -framework Syphon")

copy = env.Install("{}/bin/{}/".format(projectdir, env["platform"]), library)

framework_src = os.path.abspath("src/third_party/Syphon.xcframework/macos-arm64_x86_64/Syphon.framework")
framework_dst = "{}/bin/{}".format(projectdir, env['platform'])
copy_framework = env.Install(framework_dst, framework_src)
env.Depends(copy, copy_framework)

env.AddPostAction(
    library,
    'install_name_tool -change @rpath/Syphon.framework/Versions/A/Syphon @loader_path/Syphon.framework/Versions/A/Syphon $TARGET'
)
default_args = [library, copy]

Default(*default_args)