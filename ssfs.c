/*
  Para Compilar
  gcc -Wall ssfs.c `pkg-config fuse --cflags --libs` -o ssfs
*/

#ifdef __APPLE__
#include <stdio.h>

int
main(void)
{
  fprintf(stderr, "lol Error.\n");
  return 1;
}

#else /* !__APPLE__ */

#define FUSE_USE_VERSION 26

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef linux
/* For pread()/pwrite()/utimensat() */
#define _XOPEN_SOURCE 700
#endif

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#ifdef HAVE_SETXATTR
#include <sys/xattr.h>
#endif

static const char *dirpath = "/home/juan/.fs";

static int practica3Juan_getattr(const char *path, struct stat *stbuf)
{
  printf("1 ");
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = lstat(fpath, stbuf);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_access(const char *path, int mask)
{
  printf("2 ");
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = access(fpath, mask);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
           off_t offset, struct fuse_file_info *fi)
{
  printf("4 ");
  DIR *dp;
  struct dirent *de;

  (void) offset;
  (void) fi;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  dp = opendir(fpath);
  if (dp == NULL)
    return -errno;

  while ((de = readdir(dp)) != NULL) {
    struct stat st;
    memset(&st, 0, sizeof(st));
    st.st_ino = de->d_ino;
    st.st_mode = de->d_type << 12;
    if (filler(buf, de->d_name, &st, 0))
      break;
  }

  closedir(dp);
  return 0;
}

static int practica3Juan_mknod(const char *path, mode_t mode, dev_t rdev)
{
  printf("5 ");
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  if (S_ISREG(mode)) {
    res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
    if (res >= 0)
      res = close(res);
  } else if (S_ISFIFO(mode))
    res = mkfifo(fpath, mode);
  else
    res = mknod(fpath, mode, rdev);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_mkdir(const char *path, mode_t mode)
{
  printf("6 ");
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = mkdir(fpath, mode);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_unlink(const char *path)
{
  printf("7 ");
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = unlink(fpath);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_rmdir(const char *path)
{
  printf("8 ");
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = rmdir(fpath);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_open(const char *path, struct fuse_file_info *fi)
{
  printf("16 ");
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = open(fpath, fi->flags);
  if (res == -1)
    return -errno;

  close(res);
  return 0;
}

static int practica3Juan_read(const char *path, char *buf, size_t size, off_t offset,
        struct fuse_file_info *fi)
{
  printf("17 ");
  int fd;
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  (void) fi;
  fd = open(fpath, O_RDONLY);
  if (fd == -1)
    return -errno;

  res = pread(fd, buf, size, offset);
  if (res == -1)
    res = -errno;

  close(fd);
  return res;
}

static int practica3Juan_write(const char *path, const char *buf, size_t size,
         off_t offset, struct fuse_file_info *fi)
{
  printf("18 ");
  int fd;
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  (void) fi;
  fd = open(fpath, O_WRONLY);
  if (fd == -1)
    return -errno;

  res = pwrite(fd, buf, size, offset);
  if (res == -1)
    res = -errno;

  close(fd);
  return res;
}

static int practica3Juan_release(const char *path, struct fuse_file_info *fi)
{
  printf("20 ");
  (void) path;
  (void) fi;
  return 0;
}

static struct fuse_operations practica3Juan_operations = {
  .getattr  = practica3Juan_getattr,
  .access   = practica3Juan_access,
  .readdir  = practica3Juan_readdir,
  .mknod    = practica3Juan_mknod,
  .mkdir    = practica3Juan_mkdir,
  .unlink   = practica3Juan_unlink,
  .rmdir    = practica3Juan_rmdir,
  .open   = practica3Juan_open,
  .read   = practica3Juan_read,
  .write    = practica3Juan_write,
  .release  = practica3Juan_release,
};

int main(int argc, char *argv[])
{
  umask(0);
  system("rm -r /home/juan/.fs/*");
  practica3Juan_mkdir("/usr", 0777);
  practica3Juan_mkdir("/usr/desktop", 0777);
  practica3Juan_mkdir("/tmp", 0777);
  practica3Juan_mkdir("/etc", 0777);
  practica3Juan_mkdir("/home", 0777);
  practica3Juan_mkdir("/lib", 0777);

  char *text = "#Juan Jose Lemus Vasquez\n";
  practica3Juan_mknod("/usr/nombre_estudiante", 0777, 020);
  practica3Juan_write("/usr/nombre_estudiante", text, strlen(text), 0, NULL);

  char *text2 = "#Juan Jose Lemus Vasquez\n#201404412\n";
  practica3Juan_mknod("/home/archivo", 0777, 020);
  practica3Juan_write("/home/archivo", text2, strlen(text2), 0, NULL);
  return fuse_main(argc, argv, &practica3Juan_operations, NULL);
}

#endif /* __APPLE__ */