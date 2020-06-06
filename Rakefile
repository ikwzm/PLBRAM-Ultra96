
CC                     = "gcc"
CFLAGS                 = ""
FPGA_BITSTREAM_FILE    = "plbram_256k_dbg.bit"
DEVICE_TREE_DIRECTORY  = "plbram_256k"
DEVICE_TREE_FILE       = "plbram_256k_dbg.dts"
UDMABUF_DEVICE_NAME    = "udmabuf0"

desc "Install fpga and devicetrees"
task :install => ["/lib/firmware/#{FPGA_BITSTREAM_FILE}", DEVICE_TREE_FILE] do
  begin
    sh "./dtbocfg.rb --install #{DEVICE_TREE_DIRECTORY} --dts #{DEVICE_TREE_FILE}"
  rescue => e
    print "error raised:"
    p e
    abort
  end
  if (File.exist?("/dev/" + UDMABUF_DEVICE_NAME) == false)
    abort "can not udmabuf installed."
  end
  File::chmod(0666, "/dev/" + UDMABUF_DEVICE_NAME)
  File::chmod(0666, "/sys/class/u-dma-buf/" + UDMABUF_DEVICE_NAME + "/sync_mode")
  File::chmod(0666, "/sys/class/u-dma-buf/" + UDMABUF_DEVICE_NAME + "/sync_offset")
  File::chmod(0666, "/sys/class/u-dma-buf/" + UDMABUF_DEVICE_NAME + "/sync_size")
  File::chmod(0666, "/sys/class/u-dma-buf/" + UDMABUF_DEVICE_NAME + "/sync_direction")
  File::chmod(0666, "/sys/class/u-dma-buf/" + UDMABUF_DEVICE_NAME + "/sync_owner")
  File::chmod(0666, "/sys/class/u-dma-buf/" + UDMABUF_DEVICE_NAME + "/sync_for_cpu")
  File::chmod(0666, "/sys/class/u-dma-buf/" + UDMABUF_DEVICE_NAME + "/sync_for_device")
end

desc "Uninstall fpga and devicetrees"
task :uninstall do
  device_file = "/dev/" + UDMABUF_DEVICE_NAME
  if (File.exist?(device_file) == false)
    abort "can not #{device_file} uninstalled: does not already exists."
  end
  sh "./dtbocfg.rb --remove #{DEVICE_TREE_DIRECTORY}"
end

file "/lib/firmware/#{FPGA_BITSTREAM_FILE}" => ["#{FPGA_BITSTREAM_FILE}"] do
  sh "cp #{FPGA_BITSTREAM_FILE} /lib/firmware/#{FPGA_BITSTREAM_FILE}"
end

file "/dev/#{UDMABUF_DEVICE_NAME}" do
  Rake::Task["install"].invoke
end

file "plbram_test"    => ["plbram_test.c"] do
  sh "#{CC} #{CFLAGS} -o plbram_test plbram_test.c"
end
  
task :default => ["/dev/#{UDMABUF_DEVICE_NAME}", "plbram_test"]
