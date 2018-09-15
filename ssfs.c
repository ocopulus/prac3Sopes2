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
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = access(fpath, mask);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_readlink(const char *path, char *buf, size_t size)
{
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = readlink(fpath, buf, size - 1);
  if (res == -1)
    return -errno;

  buf[res] = '\0';
  return 0;
}


static int practica3Juan_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
           off_t offset, struct fuse_file_info *fi)
{
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
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = rmdir(fpath);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_symlink(const char *from, const char *to)
{
  int res;
  char ffrom[1000];
  sprintf(ffrom, "%s%s", dirpath, from);
  char fto[1000];
  sprintf(fto, "%s%s", dirpath, to);
  res = symlink(ffrom, fto);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_rename(const char *from, const char *to)
{
  int res;
  char ffrom[1000];
  sprintf(ffrom, "%s%s", dirpath, from);
  char fto[1000];
  sprintf(fto, "%s%s", dirpath, to);
  res = rename(ffrom, fto);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_link(const char *from, const char *to)
{
  int res;
  char ffrom[1000];
  sprintf(ffrom, "%s%s", dirpath, from);
  char fto[1000];
  sprintf(fto, "%s%s", dirpath, to);
  res = link(ffrom, fto);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_chmod(const char *path, mode_t mode)
{
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = chmod(fpath, mode);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_chown(const char *path, uid_t uid, gid_t gid)
{
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = lchown(fpath, uid, gid);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_truncate(const char *path, off_t size)
{
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = truncate(fpath, size);
  if (res == -1)
    return -errno;

  return 0;
}

#ifdef HAVE_UTIMENSAT
static int practica3Juan_utimens(const char *path, const struct timespec ts[2])
{
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = utimensat(0, fpath, ts, AT_SYMLINK_NOFOLLOW);
  if (res == -1)
    return -errno;

  return 0;
}
#endif

static int practica3Juan_open(const char *path, struct fuse_file_info *fi)
{
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

static int practica3Juan_statfs(const char *path, struct statvfs *stbuf)
{
  int res;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  res = statvfs(fpath, stbuf);
  if (res == -1)
    return -errno;

  return 0;
}

static int practica3Juan_release(const char *path, struct fuse_file_info *fi)
{
  (void) path;
  (void) fi;
  return 0;
}

static int practica3Juan_fsync(const char *path, int isdatasync,
         struct fuse_file_info *fi)
{
  (void) path;
  (void) isdatasync;
  (void) fi;
  return 0;
}

#ifdef HAVE_POSIX_FALLOCATE
static int practica3Juan_fallocate(const char *path, int mode,
      off_t offset, off_t length, struct fuse_file_info *fi)
{
  int fd;
  int res;

  (void) fi;

  if (mode)
    return -EOPNOTSUPP;
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  fd = open(fpath, O_WRONLY);
  if (fd == -1)
    return -errno;

  res = -posix_fallocate(fd, offset, length);

  close(fd);
  return res;
}
#endif

#ifdef HAVE_SETXATTR
static int practica3Juan_setxattr(const char *path, const char *name, const char *value,
      size_t size, int flags)
{
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res = lsetxattr(fpath, name, value, size, flags);
  if (res == -1)
    return -errno;
  return 0;
}

static int practica3Juan_getxattr(const char *path, const char *name, char *value,
      size_t size)
{
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res = lgetxattr(fpath, name, value, size);
  if (res == -1)
    return -errno;
  return res;
}

static int practica3Juan_listxattr(const char *path, char *list, size_t size)
{
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res = llistxattr(fpath, list, size);
  if (res == -1)
    return -errno;
  return res;
}

static int practica3Juan_removexattr(const char *path, const char *name)
{
  char fpath[1000];
  sprintf(fpath, "%s%s", dirpath, path);
  int res = lremovexattr(fpath, name);
  if (res == -1)
    return -errno;
  return 0;
}
#endif /* HAVE_SETXATTR */

static struct fuse_operations practica3Juan_operations = {
  .getattr  = practica3Juan_getattr,
  .access   = practica3Juan_access,
  .readlink = practica3Juan_readlink,
  .readdir  = practica3Juan_readdir,
  .mknod    = practica3Juan_mknod,
  .mkdir    = practica3Juan_mkdir,
  .symlink  = practica3Juan_symlink,
  .unlink   = practica3Juan_unlink,
  .rmdir    = practica3Juan_rmdir,
  .rename   = practica3Juan_rename,
  .link   = practica3Juan_link,
  .chmod    = practica3Juan_chmod,
  .chown    = practica3Juan_chown,
  .truncate = practica3Juan_truncate,
#ifdef HAVE_UTIMENSAT
  .utimens  = practica3Juan_utimens,
#endif
  .open   = practica3Juan_open,
  .read   = practica3Juan_read,
  .write    = practica3Juan_write,
  .statfs   = practica3Juan_statfs,
  .release  = practica3Juan_release,
  .fsync    = practica3Juan_fsync,
#ifdef HAVE_POSIX_FALLOCATE
  .fallocate  = practica3Juan_fallocate,
#endif
#ifdef HAVE_SETXATTR
  .setxattr = practica3Juan_setxattr,
  .getxattr = practica3Juan_getxattr,
  .listxattr  = practica3Juan_listxattr,
  .removexattr  = practica3Juan_removexattr,
#endif
};

int main(int argc, char *argv[])
{
  umask(0);
  return fuse_main(argc, argv, &practica3Juan_operations, NULL);
}

#endif /* __APPLE__ */