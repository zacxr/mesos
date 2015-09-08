/**
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __MESOS_DOCKER_HPP__
#define __MESOS_DOCKER_HPP__

#include <list>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

#include <stout/hashmap.hpp>
#include <stout/json.hpp>
#include <stout/nothing.hpp>
#include <stout/option.hpp>
#include <stout/strings.hpp>
#include <stout/try.hpp>

#include <process/future.hpp>
#include <process/owned.hpp>
#include <process/process.hpp>
#include <process/shared.hpp>

#include <mesos/resources.hpp>

#include "slave/containerizer/provisioner.hpp"

#include "slave/flags.hpp"

namespace mesos {
namespace internal {
namespace slave {
namespace docker {

// Forward declaration.
class Store;

struct ImageName
{
  std::string repo;
  std::string tag;
  Option<std::string> registry;

  ImageName(const std::string& name);

  ImageName(
      const std::string& repo,
      const std::string& tag,
      const Option<std::string>& registry = None())
    : repo(repo), tag(tag), registry(registry) {}

  ImageName() {}
};


inline std::ostream& operator<<(std::ostream& stream, const ImageName& image)
{
  if (image.registry.isSome()) {
    return stream << image.registry.get()
                  << "/" << image.repo << ":" << image.tag;
  }
  return stream << image.repo << ":" << image.tag;
}


struct DockerImage
{
  DockerImage() {}

  DockerImage(
      const std::string& imageName,
      const std::list<std::string>& layers)
  : imageName(imageName), layers(layers) {}

  std::string imageName;
  std::list<std::string> layers;
};

// Forward declaration.
class DockerProvisionerProcess;

class DockerProvisioner : public Provisioner
{
public:
  static Try<process::Owned<Provisioner>> create(
      const Flags& flags,
      Fetcher* fetcher);

  virtual ~DockerProvisioner();

  virtual process::Future<Nothing> recover(
      const std::list<mesos::slave::ContainerState>& states,
      const hashset<ContainerID>& orphans);

  virtual process::Future<std::string> provision(
      const ContainerID& containerId,
      const Image& image);

  virtual process::Future<bool> destroy(const ContainerID& containerId);

private:
  explicit DockerProvisioner(process::Owned<DockerProvisionerProcess> process);
  DockerProvisioner(const DockerProvisioner&); // Not copyable.
  DockerProvisioner& operator=(const DockerProvisioner&); // Not assignable.

  process::Owned<DockerProvisionerProcess> process;
};


} // namespace docker {
} // namespace slave {
} // namespace internal {
} // namespace mesos {

#endif // __MESOS_DOCKER_HPP__
