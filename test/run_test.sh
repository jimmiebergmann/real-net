program="../bin/test-d"
error_kinds="definite,indirect"
warning_kinds="definite,indirect"

script="valgrind --leak-check=full --track-origins=yes --errors-for-leak-kinds=${error_kinds} --show-leak-kinds=${warning_kinds} --run-libc-freeres=no --error-exitcode=1 ./${program}"

echo "Running:"
echo ${script}
${script}
