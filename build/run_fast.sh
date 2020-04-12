make

if [ $? -eq 0 ]; then
    ./strategy

    echo "Exit with code " $?
else
    exit 1
fi

