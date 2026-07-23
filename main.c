#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>

int main(void)
{
    int fd = open("/dev/sr0", O_RDONLY);
    if (fd < 0) {
        perror("open");
        return 1;
    }

    struct cdrom_tochdr tochdr;
    int ret = ioctl(fd, CDROMREADTOCHDR, &tochdr);
    if (ret == -1) {
        perror("ioctl CDROMREADTOCHDR");
        close(fd);
        return 1;
    }

    printf("First track: %u\n", tochdr.cdth_trk0);
    printf("Last track: %u\n", tochdr.cdth_trk1);

    for (int track = tochdr.cdth_trk0; track <= tochdr.cdth_trk1; ++track) {

        struct cdrom_tocentry entry;
        entry.cdte_track = (unsigned char)track;
        entry.cdte_format = CDROM_MSF;

        ret = ioctl(fd, CDROMREADTOCENTRY, &entry);
        if (ret == -1) {
            perror("ioctl CDROMREADTOCENTRY");
            close(fd);
            return 1;
        }

        printf("Track %u starts at %u:%u\n",
               entry.cdte_track,
               entry.cdte_addr.msf.minute,
               entry.cdte_addr.msf.second);
    }

    struct cdrom_tocentry leadout;
    leadout.cdte_track = CDROM_LEADOUT;
    leadout.cdte_format = CDROM_MSF;
    ret = ioctl(fd, CDROMREADTOCENTRY, &leadout);
    if (ret == -1) {
        perror("ioctl CDROMREADTOCENTRY leadout");
        close(fd);
        return 1;
    }

    printf("Track %u (lead-out) starts at %u:%u\n",
           leadout.cdte_track,
           leadout.cdte_addr.msf.minute,
           leadout.cdte_addr.msf.second);

    close(fd);
    return 0;
}
