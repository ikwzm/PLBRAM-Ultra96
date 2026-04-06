require 'yaml'
ENV_YAML_FILE = "Rakefile.env"
if File.exist?(ENV_YAML_FILE)
  env = open(ENV_YAML_FILE, 'r'){ |f| YAML.load(f) }
else
  env = {}
end
if ENV.key?("TARGET")
  env["TARGET"] = ENV["TARGET"]
  YAML.dump(env, File.open(ENV_YAML_FILE, 'w'))
end
if ENV.key?("BASE_DTS")
  env["BASE_DTS"] = ENV["BASE_DTS"]
  YAML.dump(env, File.open(ENV_YAML_FILE, 'w'))
end
TARGET                 = env.fetch("TARGET"  , "plbram_256k_dbg")
BASE_DEVICE_TREE_FILE  = env.fetch("BASE_DTS", "plbram_v2.dts")

CC                     = "gcc"
CFLAGS                 = ""
FPGA_BITSTREAM_FILE    = TARGET + ".bin"
FPGA_BITSTREAM_GZ_FILE = FPGA_BITSTREAM_FILE + ".gz"
DEVICE_TREE_FILE       = TARGET + ".dts"
DEVICE_TREE_NAME       = "plbram_256k"
DEVICE_TREE_DIRECTORY  = "/config/device-tree/overlays/" + DEVICE_TREE_NAME
UIOMEM_DEVICE_NAME     = "uiomem0"
DTBOCFG                = "./dtbo-config"

require 'rake/clean'

desc "Install fpga and devicetrees(#{DEVICE_TREE_NAME})"
task :install => ["/lib/firmware/#{FPGA_BITSTREAM_FILE}", DEVICE_TREE_FILE] do
  begin
    sh "#{DTBOCFG} --install #{DEVICE_TREE_NAME} --dts #{DEVICE_TREE_FILE}"
  rescue => e
    print "error raised:"
    p e
    abort
  end
  if (File.exist?("/dev/" + UIOMEM_DEVICE_NAME) == false)
    abort "can not uiomem installed."
  end
  File::chmod(0666, "/dev/" + UIOMEM_DEVICE_NAME)
  File::chmod(0666, "/sys/class/uiomem/" + UIOMEM_DEVICE_NAME + "/sync_mode")
  File::chmod(0666, "/sys/class/uiomem/" + UIOMEM_DEVICE_NAME + "/sync_offset")
  File::chmod(0666, "/sys/class/uiomem/" + UIOMEM_DEVICE_NAME + "/sync_size")
  File::chmod(0666, "/sys/class/uiomem/" + UIOMEM_DEVICE_NAME + "/sync_direction")
  File::chmod(0666, "/sys/class/uiomem/" + UIOMEM_DEVICE_NAME + "/sync_owner")
  File::chmod(0666, "/sys/class/uiomem/" + UIOMEM_DEVICE_NAME + "/sync_for_cpu")
  File::chmod(0666, "/sys/class/uiomem/" + UIOMEM_DEVICE_NAME + "/sync_for_device")
end

desc "Uninstall fpga and devicetrees(#{DEVICE_TREE_NAME})"
task :uninstall do
  if (Dir.exist?(DEVICE_TREE_DIRECTORY) == false)
    abort "can not #{DEVICE_TREE_DIRECTORY} uninstalled: does not already exists."
  end
  sh "#{DTBOCFG} --remove #{DEVICE_TREE_NAME}"
end

file "/lib/firmware/" + FPGA_BITSTREAM_FILE => [ FPGA_BITSTREAM_GZ_FILE ] do
  sh "gzip -d -f -c #{FPGA_BITSTREAM_GZ_FILE} > /lib/firmware/#{FPGA_BITSTREAM_FILE}"
end
CLOBBER.include("/lib/firmware/" + FPGA_BITSTREAM_FILE)

directory DEVICE_TREE_DIRECTORY do
  Rake::Task["install"].invoke
end

file DEVICE_TREE_FILE => [ BASE_DEVICE_TREE_FILE ] do
  File.open(DEVICE_TREE_FILE, "w") do |o_file|
    File.open(BASE_DEVICE_TREE_FILE) do |i_file|
      i_file.each_line do |line|
        line = line.gsub(/(^\s*firmware-name\s*=\s*)(.*);/){"#{$1}\"#{FPGA_BITSTREAM_FILE}\";"}
        o_file.puts(line)
      end
    end
  end
end

task :default => ["/dev/#{UIOMEM_DEVICE_NAME}"]
