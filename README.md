Droplet backup is an incremental command line backup agent that can talk to
cloud storage service providers.

It's based on the Droplet cloud client library and is compatible with all
services providers and protocols supported by Droplet. This includes:
HostEurope, SeaWeb, Dunkel, Tiscali, Amazon S3.

For more information see https://github.com/scality/Droplet

Main features:

  * Incremental backup
  * Multiple backup repositories
  * Block level dedup with intelligent variable window size algorithm


    DPLBCK(1)                                                DPLBCK(1)
    
    NAME
           dplbck - backup a local folder tree on a remote storage
    
    SYNOPSIS
           dplbck < command > [ options ] < args >
    
    DESCRIPTION
           dplbck  makes  an  incremental  backup of a local tree on a
           remote storage and restores it back.
    
    COMMANDS
           backup Make a backup.
    
           restore
                  Restore a backup.
    
           delete Delete a backup.
    
           purge  Purge unused blocks of a backup directory.
    
           list   List all available backups in a backup directory.
    
           help   Display detailed help about each command.
    
    STORAGE TYPES
           dplbck supports multiple storage types and will adapt using
           the  right  module according to the URI scheme specified on
           the command line. Supported storage backends are:
    
           dpl:// to backup to an S3 compatible cloud storage.
    
           file://
                  to backup to a local directory.
    
    RETURN VALUE
           dplbck returns 0 on success, and non-zero on error.
    
    BUGS
           Please send problems, bugs, questions,  desirable  enhance‐
           ments, patches etc. to sas@cd80.net.
    
    AUTHOR
           Stephane Sezer <sas@cd80.net>
    
    SEE ALSO
           dplbck-backup(1),    dplbck-restore(1),    dplbck-delete(1)
           dplbck-purge(1) dplbck-list(1)
    
                                   2011                      DPLBCK(1)

