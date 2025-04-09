"""Script to create a new release and tag on GitLab.

This script is run on GitLab CI. A new release will be created if the version number in project.m is not already a git
tag. This is only triggered on a push or merge to the main branch.
"""
import re
import os
import subprocess
import requests
import zipfile


def main():
    version = get_current_version()
    existing_versions = get_all_versions()
    if version not in existing_versions:
        print(f"Creating tag and release for new version {version}.")
        tag = f'v{version}'
        title = f'Version {version}'
        description = get_changelog_for_version(version)
        file = "release/BeamNG.tech-Support-for-MATLAB-and-Simulink.mltbx"
        package_name = "toolbox"
        verify_release_file_version(file, version)
        filename, url = upload_artifact(file, package_name, version)
        links = [{'name': filename, 'url': url}]
        create_release(tag, title, description, links)
    else:
        print(f"Version {version} already exists. Nothing to do.")


def get_current_version() -> str:
    """Get version without the v prefix."""
    file = 'project.m'
    with open(file, 'r') as f:
        for line in f:
            m = re.match(r'.*ToolboxVersion = \'(\d+\.\d+\.\d+(\.\d+)?)\'.*', line)
            if m:
                version = m.group(1)
                return version
    raise ValueError(f'ToolboxVersion not found in {file}')


def get_all_versions() -> set[str]:
    """Get all versions from git tags without the v prefix."""
    # get all tags from git
    tags = subprocess.check_output(['git', 'tag']).decode('utf-8').split('\n')
    # use only tags that start start with v followed by a number, stripping the v
    tags = [tag[1:] for tag in tags if re.match(r'^v\d+\..*$', tag)]
    return set(tags)


def get_changelog_for_version(version: str) -> str:
    """Get the changelog for a given version."""
    lines = []
    started = False
    with open('CHANGELOG.md', 'r') as f:
        for line in f:
            if not started and line == f'## Version {version}\n':
                started = True
                continue
            if started and line.startswith('##'):
                break
            if started:
                lines.append(line)
    if len(lines) == 0:
        raise ValueError(f'Changelog for version {version} not found')
    return ''.join(lines)


def verify_release_file_version(filepath: str, version: str):
    """Verify that the version in the release file matches the version in project.m."""
    read_version = read_release_file_version(filepath)
    if read_version != version:
        raise ValueError(f'Version in {filepath} ({read_version}) does not match version in project.m ({version})')


def read_release_file_version(filepath: str) -> str:
    """Read the version from the release file."""
    internal_path = 'metadata/coreProperties.xml'
    with zipfile.ZipFile(filepath, 'r') as zip_ref:
        with zip_ref.open(internal_path, 'r') as file:
            for line in file:
                match = re.search(r'\w*<cp:version>([\d\.]+)</cp:version>\w*', line.decode('utf-8'))
                if match:
                    return match.group(1)
    raise ValueError(f'Version not found in {filepath}')


def upload_artifact(filepath: str, package_name: str, version: str) -> str:
    """Upload an artifact to the project's generic package registry."""
    api_url = os.environ['CI_API_V4_URL']
    project_id = os.environ['CI_PROJECT_ID']
    filename = os.path.basename(filepath)
    file_size = os.path.getsize(filepath)
    url = f'{api_url}/projects/{project_id}/packages/generic/{package_name}/{version}/{filename}'
    headers = {
        'JOB-TOKEN': os.environ['CI_JOB_TOKEN'],
        'Content-Type': 'application/octet-stream',
        'Content-Length': str(file_size),
    }
    with open(filepath, 'rb') as f:
        response = requests.put(url, headers=headers, data=f)
    response.raise_for_status()
    print(f'Artifact {filename} uploaded to {url}')
    return filename, url


def create_release(tag: str, title: str, description: str, links: list[dict]):
    """Use GitLab API to create a release."""
    api_url = os.environ['CI_API_V4_URL']
    project_id = os.environ['CI_PROJECT_ID']
    ref = os.environ['CI_COMMIT_SHA']
    url = f'{api_url}/projects/{project_id}/releases'
    headers = {
        'JOB-TOKEN': os.environ['CI_JOB_TOKEN']
    }
    data = {
        'tag_name': tag,
        'name': title,
        'description': description,
        'ref': ref,
        'assets': {
            'links': links
        }
    }
    response = requests.post(url, headers=headers, json=data)
    response.raise_for_status()
    print(f'Release {tag} created successfully.')


if __name__ == '__main__':
    main()
