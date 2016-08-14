# Creates a compile_commands.json compilation database using bazel aspects

def _compilation_database_aspect_impl(target, ctx):
    sources = [f for src in ctx.rule.attr.srcs for f in src.files]
    if 'hdrs' in dir(ctx.rule.attr):
      sources.extend([h for src in ctx.rule.attr.hdrs for h in src.files])

    compilation_infos = []
    for s in sources:
      flags = target.cc.compile_flags
      obj = s.path
      for ending in ['.h', '.cc', '.c', '.cpp']:
          obj = obj.replace(ending, '.o')
      flags.extend('-x c++ -std=c++11 -isystem /hepgpu1-data3/ibabusch/.anaconda/envs/tf/gcc/include/c++/ -isystem /hepgpu1-data3/ibabusch/.anaconda/envs/tf/gcc/include/c++/x86_64-unknown-linux-gnu'.split(' '))
      commandline = '/usr/bin/g++ %s -c -o %s %s' % (" ".join(flags), obj, s.path)
      # TODO Find out what the proper solution is here.
      #real_root = str(s.root).split('[')[0] + '/execroot/laminate'
      real_root = "/hepgpu1-data3/ibabusch/.cache/bazel/_bazel_ibabusch/061c6647b6ddf3ddf795d5c75aaec132/execroot/laminate"
      compilation_infos.append(struct(file=s.path, command=commandline, directory=real_root))

    for d in ctx.rule.attr.deps:
      compilation_infos += d.compilation_infos

    return struct(compilation_infos=compilation_infos)

def _compilation_database_impl(ctx):
    all_infos = []
    for t in ctx.attr.targets:
        all_infos += t.compilation_infos

    entries = [e.to_json() for e in all_infos]
    ctx.file_action(output=ctx.outputs.filename, content="[\n " + ", \n ".join(entries) + "\n]")

compilation_database_aspect = aspect(
    implementation = _compilation_database_aspect_impl,
    attr_aspects = ["deps"],
)

compilation_database = rule(
    implementation = _compilation_database_impl,
    attrs = {
        'targets': attr.label_list(aspects = [compilation_database_aspect])
    },
    outputs = {
        'filename': 'compile_commands.json',
    }
)
