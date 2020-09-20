# An automatic installation script that installs the latest stable version of LLVM:

	sudo su
	bash -c "$(wget -O - https://apt.llvm.org/llvm.sh)"

	deb http://apt.llvm.org/bionic/ llvm-toolchain-bionic-10 main
	wget -O - https://apt.llvm.org/llvm-snapshot.gpg.key|sudo apt-key add -
	apt-get update
	apt install clang
	clang --version

	nano llvm_test.c
	clang llvm_test.c -o llvm_test_code
	./llvm_test_code