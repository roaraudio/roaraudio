#
# spec file for package: roaraudio
#
# This file and all modifications and additions to the pristine
# package are under the same license as the package itself.
#
# includes module(s): [pkg module(s)]
#

%include Solaris.inc

Name:		roaraudio
Summary:      	RoarAudio is a cross-platform sound system for both, home and professional use.
Version:       	0.2rc1
License:	GPLv3, LGPLv3
Url: 		http://roaraudio.keep-cool.org/
Source:	 	http://roaraudio.keep-cool.org/dl/%{name}-%{version}.tar.gz
Group:		[ips pkg classification]
Distribution:   OpenSolaris
Vendor:		OpenSolaris Community
BuildRoot:      %{_tmppath}/%{name}-%{version}-build
SUNW_Basedir:   %{_basedir}
SUNW_Copyright: %{name}.copyright

%include default-depend.inc

# OpenSolaris IPS Package Manifest Fields
Meta(info.upstream):	 	[name email of open source project leader]
Meta(info.maintainer):	 	[name email of ips pkg porter/maintainer]
Meta(info.repository_url):	[open source code repository]

%description
[long description]

%prep
[pre-build commands]

%build
[build commands]

%install
[install commands]

%clean
[cleanup commands]

%files
[file groups and permissions]

%changelog
[change log entries]
