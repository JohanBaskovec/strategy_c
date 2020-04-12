cmake ..
make

if [ $? -eq 0 ]; then
    ./pong_test

    if [ $? -eq 0 ]; then
        echo "Test suite success." $?
        exit 0
    else
        echo "Test suite failed!" $?
        exit 1
    fi
else
    exit 1
fi

