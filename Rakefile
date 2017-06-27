 require 'rake/testtask'
require 'rake/clean'

require 'fileutils'
require 'open3'
require 'yaml'


require './common'
# ^ this file declares GEM_ROOT constant, other constants, and a some functions



# invoke a particular rake task by name (and then allow it to be run again later)
def run_task(task)
	Rake::Task[task].reenable
	Rake::Task[task].invoke
	# Rake::Task[task].reenable
	# src: http://stackoverflow.com/questions/577944/how-to-run-rake-tasks-from-within-rake-tasks
end


# temporarily swap out the makefile for an alternate version
# 
# main_filepath, alt_filepath:  Paths to main and alt makefile, relative to common_root.
# common_root:                  As above.
# work_dir:                     Path under which to run the commands specified in the block.
def swap_makefile(common_root, main_filepath, alt_filepath, &block)
	swap_ext = ".temp"
	swap_filepath = File.join(common_root, "Makefile#{swap_ext}")
	
	
	main_filepath = File.expand_path(File.join(common_root, main_filepath))
	alt_filepath  = File.expand_path(File.join(common_root, alt_filepath))
	
	
	
	
	# run tasks associated with the alternate file
	begin
		FileUtils.mv main_filepath, swap_filepath # rename main makefile
		FileUtils.cp alt_filepath, main_filepath  # switch to .a-creating mkfile
		
		block.call
	ensure
		FileUtils.cp swap_filepath, main_filepath # restore temp
		FileUtils.rm swap_filepath                # delete temp		
		# I think this ensure block should make it so the Makefile always restores,
		# even if there is an error in the block.
		# src: http://stackoverflow.com/questions/2191632/begin-rescue-and-ensure-in-ruby
	end
end





# generate depend file for gcc dependencies
# sh "gcc -MM *.c > depend"





# use 'rake clean' and 'rake clobber' to
# easily delete generated files

# NOTE: Clean / clobber tasks may accidentally clobber oF dependencies if you are not careful.

CLEAN.include('ext/**/*{.o,.log,.so}')
CLEAN.include('ext/**/*{.a}')
CLEAN.include('ext/rubyOF/Makefile')
	# c1 = CLEAN.clone
	# p CLEAN
CLEAN.exclude('ext/openFrameworks/**/*')
# ^ remove the openFrameworks core
	# c2 = CLEAN.clone
	# p CLEAN
# CLEAN.exclude('ext/oF_apps/**/*')
# # ^ remove the test apps as well


CLOBBER.include('lib/**/*.so')
	# c3 = CLOBBER.clone
	# p CLOBBER
# CLOBBER.include('lib/**/*.gem') # fix this up. I do want to clobber the gem tho

	# require 'irb'
	# binding.irb

	# exit
	# raise "WHOOPS"






namespace :oF do
	desc "Download openFrameworks libraries (build deps)"
	task :download_libs do
		run_i "ext/openFrameworks/scripts/linux/download_libs.sh"
		# ^ this script basically just triggers another script:
		#   ext/openFrameworks/scripts/dev/download_libs.sh
	end
	
	# NOTE: If there is a problem with the core, try downloading libs again.
	# NOTE: If there is a problem with building the oF project, download libs again, build the core again, and then rebuild the project.
	desc "Build openFrameworks core (ubuntu)."
	task :build do
		Dir.chdir "./ext/openFrameworks/scripts/linux/" do
			run_i "./compileOF.sh -j#{NUMBER_OF_CORES}"
		end
	end
	
	
	# NOTE: Project generator can update existing projects, including specifying the addons used for a particular project.
	desc "Use the openFrameworks project generator to create a new project in the correct directory."
	task :project_generator, [:oF_project_name] do |t, args|
		project = args[:oF_project_name]
		
		if project.nil?
			raise "ERROR: must specify oF_project_name"
		end
		
		
		# NOTE: These paths need to be full paths, because they are being passed to another system, which is executing from a different directory.
		dir = "ext/openFrameworks/apps/projectGenerator/commandLine/bin"
		full_dir = File.expand_path dir, GEM_ROOT
		
		
		a = File.join(GEM_ROOT, "ext", "openFrameworks")
		b = File.join(GEM_ROOT, "ext", "oF_apps", project)
		
		Dir.chdir full_dir do
			# p Dir.pwd
			
			run_i "./projectGenerator -o\"#{a}\" #{b}" 
		end
		
	end
	
end

namespace :oF_project do
	desc "Run just the C++ components for the oF sketch"
	task :run => :build do
		Dir.chdir OF_SKETCH_ROOT do
			run_i "make RunRelease"
		end
	end
	
	desc "Build the oF project (C++ only) - generates .o files"
	task :build do
		Dir.chdir OF_SKETCH_ROOT do
			run_i "make -j#{NUMBER_OF_CORES}" do
				"ERROR: Could not build oF sketch."
			end
		end
	end
	
	desc "Clean the oF project (C++ only) [also cleans addons]"
	task :clean do
		Dir.chdir OF_SKETCH_ROOT do
			run_i "make clean"
		end
	end
	
	
	
	# rebuilding the project should rebuild the addons too
	desc "Rebuld the project."
	task :rebuild do
		run_task('oF_sketch:clean')
		run_task('oF_sketch:build')
	end
	
	
	
	
	# show the .o files generated that are specific to this project
	# (these are the files used to generate the static lib)
	desc "DEBUG: show the .o files generated that are specific to this project"
	task :examine do
		path = File.expand_path("obj/#{PLATFORM}/#{TARGET}/src", OF_SKETCH_ROOT)
		Dir.chdir path do
			puts "local oF build directory:"
			puts Dir.pwd
			p Dir['./*']
		end
	end
end




# === Build the C extension
namespace :c_extension do
	# make the :test task depend on the shared
	# object, so it will be built automatically
	# before running the tests
	
	# rule to build the extension: this says
	# that the extension should be rebuilt
	# after any change to the files in ext
	
	c_library = "lib/#{NAME}/#{NAME}.so"
	
	
	# TODO: update source file list
	source_files = Array.new
	# source_files += Dir.glob("ext/#{NAME}/*{.rb,.c}")
	
	
	# Make sure the Ruby C extension is dependent on the oF sketch.
	# This way, when the sketch is altered, changes will propogate to the Ruby-level.
	source_files << File.expand_path("./bin/#{OF_SKETCH_NAME}", OF_SKETCH_ROOT)
	# ^ This seems to work. Why does the system detect changes to the executable, but not the library?
	# 
		# So it looks like the build system links against the raw .o files generated by the open frameworks build system.
		# nothing in extconf.rb references the libOFSketch.a *at all*
		# that file has not been touched in forever, it's not being linked into anything,
		# it's basically no use to anyone.

		# I think what happened is I thought I needed this .a as an intermediate step, but it confused me.
		# So I ended up dropping the whole thing, and just using Ruby's facilities to figure it out?
		# rather than having to write makefiles
		# the following is quoted from a comment in 'Makefile.static_lib'
		# ----
			# this doesn't seem to work. might have to do this in a rakefile, because that's something I actually understand, lol.
			# these variables are coming up empty, and I'm not sure why.
		# ----
	# HOWEVER: The vestigial libOFSketch.a generation code can not be discarded.
		# While the library is vestigial, that part of the build process
		# generates 'oF_build_variables.yaml', which is used by extconf.rb to
		# configure various flags that should be passed to the complier / linker.
	# TODO: Re-examine the history of creating the build system, and attempt to refactor such that this (largely) vestigital pathway can actually be removed safely.
	
	# Ruby / Rice CPP files
	source_files += Dir.glob("ext/#{NAME}/cpp/lib/**/*{.cpp,.h}")
	
	# 
	source_files << "ext/#{NAME}/extconf.rb"
	source_files << __FILE__
	
	# source_files << OF_BUILD_VARIABLE_FILE
	# TODO: ^ re-enable this ASAP
	
	# NOTE: adding OF_BUILD_VARIABLE_FILE to the dependencies for the 'c_library' makes it so extconf.rb has to run every time, because the variable file is being regenerated every time.
	
	
	# FIXME: can't seem to just run rake again to rebuild. keep having to clobber, and then do a full clean build again.
	
	
	
	
	file OF_BUILD_VARIABLE_FILE => 
	[
		File.expand_path("./Makefile.static_lib", OF_SKETCH_ROOT),
		File.expand_path("./addons.make",         OF_SKETCH_ROOT),
		__FILE__, # if the Rake task changes, then update the output file
		COMMON_CONFIG # if config variables change, then build may be different
	] do
		run_task 'oF_sketch_lib:print_vars'
	end
	
	
	# Mimic RubyGems gem install procedure, for testing purposes.
	# * run extconf
	# * execute the resultant makefile
	# * move the .so to it's correct location
	file c_library => source_files do
		Dir.chdir("ext/#{NAME}") do
			# this does essentially the same thing
			# as what RubyGems does
			puts "=== starting extconf..."
			
			run_i "ruby extconf.rb" do
				"ERROR: Could not configure c extension."
			end
			
			
			puts "======= Top level Rakefile"
			puts "=== configuration complete. building C extension"
			
			
			# Run make
			
			flags = ""
			# flags += " -j#{NUMBER_OF_CORES}" if Dir.exists? '/home/ravenskrag' # if running on my machine
			
			
			run_i "make #{flags}"  do
				"ERROR: Could not build c extension."
			end
		end
		
		puts "=== Moving dynamic library into correct location..."
		cp "ext/#{NAME}/#{NAME}.so", "lib/#{NAME}"
		
		
		puts "=== C extension build complete!"
	end
	
	desc "Build the C extension"
	task :build => c_library
	
	
	
	clang_symbol_file = "/ext/#{NAME}/compile_commands.json"
	
	# Not sure how often you want to regenerate this file, but not every time you build.
	# You need to run make and have something happen. If nothing gets build from the makefile, the clang database will end up empty.
	file clang_symbol_file do
		Dir.chdir("ext/#{NAME}") do
			run_i "make clean"
			
			run_i "bear make" do
				"ERROR: Had a problem when Bear tried to examine the make process"
			end
		end
	end
	
	desc "Generate clang symbol DB"
	task :build_clang_db do
		run_task(clang_symbol_file)
	end
	
	# TODO: make sure the clang symbols are generated as part of the standard build process
	# TODO: add clang symbols file to the .gitignore. Should be able to generate this, instead of saving it.
	
	
end

desc "Build the whole project"
task :build => ['c_extension:build']




