
int main(int argc, char **argv)
{
    /**
     * Configure GPIO directions
     * Initialize camera
     */
    while (1) {
        /**
         * read from camera
         * read any commands (nonblocking)
         *   weave [right|left] [%d times -1 for infinity] [%f percent wideness]
         *   bob [right|left] [%d times -1 for infinity] [%f percent wideness]
         *   spiral [cw|ccw] [%d times] [%f percent wideness, 100% is very wide]
         *   dive [%f percent down - 100% means hit the floor] [right|left - curve out at bottom] [%f dive angle, -90 is straight down]
         *   eights [cw|ccw] [%d times -1 for infinity]
         *   MORE (pending)
         * 
         * write commands to pikelo (shm)
         */
        mysleep(mymilliseconds(250));
    }

    return 0;
}
